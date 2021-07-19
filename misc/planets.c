/*
    A rough calculation of planetary motion.
    To test various floating point packages available for CPM and other z88dk supported targets.

    build with:

    zcc +test -v -O2 --list -lm -DPRINTF planets.c -o planetgm_sccz80_ticks.bin
    zcc +test -v -O2 --list -lmath48 -DPRINTF planets.c -o planet48_sccz80_ticks.bin
    zcc +test -v -O2 --list --math32 -DPRINTF planets.c -o planet32_sccz80_ticks.bin

    zcc +test -compiler=sdcc -v -SO3 --list --reserve-regs-iy -lmath48 -DPRINTF --max-allocs-per-node100000 planets.c -o planet48_sdcc_ticks.bin
    zcc +test -compiler=sdcc -v -SO3 --list --reserve-regs-iy --math32 -DPRINTF --max-allocs-per-node100000 planets.c -o planet32_sdcc_ticks.bin

    z88dk-ticks -counter 99999999999 planetxx_x_ticks.bin

    zcc +cpm -clib=new -v -O2 --list -lm -DPRINTF planets.c -o planetnew_cpm -create-app
    zcc +cpm -clib=sdcc_iy -m --list  -lm     -DPRINTF --max-allocs-per-node100000 planets.c -o planet48_cpm -create-app
    zcc +cpm -clib=sdcc_iy -m --list --math32 -DPRINTF --max-allocs-per-node100000 planets.c -o planet32_cpm -create-app
    
    Based on the work of [Paul Schlyter](http://www.stjarnhimlen.se/english.php).

     - [How to compute planetary postitions](http://www.stjarnhimlen.se/comp/ppcomp.html).
     - [Tutorial on computing planetary positions](http://www.stjarnhimlen.se/comp/tutorial.html).

    And the implementations of [Cosine Kitty (Don Cross)](http://cosinekitty.com/)

     - [Solar System Calculator](https://cosinekitty.com/solar_system.html).
     - [Astronomy Sky View](http://cosinekitty.com/sky_view.html).
 */

/*
 *  40 calculations for 9 bodies on z88dk-ticks - reduced printing.
 *
 *  sccz80/classic/genmath  Ticks: 964825976
 *  sccz80/new/math48       Ticks: 765546928
 *  sccz80/new/math32       Ticks: 242514534
 *
 *  sdcc/new/math48         Ticks: 734003287
 *  sdcc/new/math32         Ticks: 246529328
 *
 */

#include <stdint.h>
#include <math.h>
#include <stdio.h>

#ifdef PRINTF
   #define FPRINTF(a,b,c,d,e)   printf(a,b,c,d,e)
#else
   #define FPRINTF(a,b,c,d,e)
#endif

// numeric constants...

#define METERS_PER_ASTRONOMICAL_UNIT        1.4959787e+11
#define METERS_PER_EARTH_EQUATORIAL_RADIUS  6378160.0
#define EARTH_RADII_PER_ASTRONOMICAL_UNIT   (METERS_PER_ASTRONOMICAL_UNIT/METERS_PER_EARTH_EQUATORIAL_RADIUS)

// macros

#define FLOAT       float_t

#define FABS        fabs
#define FLOOR       floor

#define SQRT        sqrt
#define HYPOT       hypot

#define COS         cos
#define SIN         sin
#define TAN         tan
#define ACOS        acos
#define ASIN        asin
#define ATAN        atan
#define ATAN2       atan2

#if defined(__MATH_MATH32)
    #define SQR(x)  sqr(x)
#else
    #define SQR(x)  ((x)*(x))
#endif

#define RAD(x)      ((x)*(M_PI/180.0))
#define DEG(x)      ((x)*(180.0/M_PI))

// pragmas

#pragma printf = "%10s %.3f"     // enables %s, %f only

// type definitions

typedef struct cartesian_coordinates_s {
    FLOAT x;
    FLOAT y;
    FLOAT z;
    FLOAT day;
} cartesian_coordinates_t;

typedef struct planet_s {   // See  http://www.stjarnhimlen.se/comp/ppcomp.html#4
    char * name;            // name of the object, e.g. "Mars".
    FLOAT N0, Nc;           // N0 = longitude of the ascending node (deg).  Nc = rate of change in deg/day.
    FLOAT i0, ic;           // inclination to the ecliptic (deg).
    FLOAT w0, wc;           // argument of perihelion (deg).
    FLOAT a0, ac;           // semi-major axis, or mean distance from Sun (AU).
    FLOAT e0, ec;           // eccentricity (0=circle, 0..1=ellipse, 1=parabola).
    FLOAT M0, Mc;           // M0 = mean anomaly (deg) (0 at perihelion; increases uniformly with time).  Mc ("mean motion") = rate of change in deg/day = 360/period.
    FLOAT radius;           // radius proportional to earth (earth = 1.0)
} planet_t;

// utility functions

void sunEclipticCartesianCoordinates ( cartesian_coordinates_t * sun ) __z88dk_fastcall;
void planetEclipticCartesianCoordinates ( cartesian_coordinates_t * location, const planet_t * planet ) __z88dk_callee;
FLOAT eccentricAnomaly (FLOAT e, FLOAT M) __z88dk_callee;
void addCartesianCoordinates ( cartesian_coordinates_t * base, cartesian_coordinates_t * addend ) __z88dk_callee;
FLOAT rev (FLOAT x) __z88dk_fastcall;

// constant values

const planet_t sun =      { "Sun", \
                            0.0, 0.0, \
                            0.0, 0.0, \
                            282.9404, 4.70935E-5, \
                            1.0, 0.0, \
                            0.016709, -1.151E-9, \
                            356.0470, 0.9856002585, \
                            (695500/6378) };

const planet_t moon =     { "Moon", \
                            125.1228, -0.0529538083, \
                            5.1454, 0.0, \
                            318.0634, 0.1643573223, \
                            60.2666/EARTH_RADII_PER_ASTRONOMICAL_UNIT, 0.0, \
                            0.054900, 0.0, \
                            115.3654, 13.0649929509, \
                            (1738/6378) };


const planet_t mercury =  { "Mercury", \
                            48.3313, 3.24587e-5, \
                            7.0047, 5.0e-8, \
                            29.1241, 1.01444e-5, \
                            0.387098, 0.0, \
                            0.205635, 5.59e-10, \
                            168.6562, 4.0923344368, \
                            (2440/6378) };

const planet_t venus =    { "Venus", \
                            76.6799, 2.46590e-5, \
                            3.3946, 2.75e-8, \
                            54.8910, 1.38374e-5, \
                            0.723330, 0.0, \
                            0.006773, -1.302e-9, \
                            48.0052, 1.6021302244, \
                            (6052/6378) };

const planet_t mars =     { "Mars", \
                            49.5574, 2.11081e-5, \
                            1.8497, -1.78e-8, \
                            286.5016, 2.92961e-5, \
                            1.523688, 0.0, \
                            0.093405, 2.516e-9, \
                            18.6021, 0.5240207766, \
                            (3390/6378) };

const planet_t jupiter =  { "Jupiter", \
                            100.4542, 2.76854E-5, \
                            1.3030, - 1.557E-7, \
                            273.8777, 1.64505E-5, \
                            5.20256, 0.0, \
                            0.048498, 4.469E-9, \
                            19.8950, 0.0830853001, \
                            (69911/6378) };

const planet_t saturn =   { "Saturn", \
                            113.6634, 2.3898e-5, \
                            2.4886, -1.081e-7, \
                            339.3939, 2.97661e-5, \
                            9.55475, 0.0, \
                            0.055546, -9.499e-9, \
                            316.9670, 0.0334442282, \
                            (58232/6378) };

const planet_t uranus =   { "Uranus", \
                            74.0005, 1.3978E-5, \
                            0.7733, 1.9E-8, \
                            96.6612, 3.0565E-5, \
                            19.18171, - 1.55E-8, \
                            0.047318, 7.45E-9, \
                            142.5905, 0.011725806, \
                            (25362/6378) };

const planet_t neptune =  { "Neptune", \
                            131.7806, 3.0173e-5, \
                            1.7700, -2.55e-7, \
                            272.8461, -6.027e-6, \
                            30.05826, 3.313e-8, \
                            0.008606, 2.15e-9, \
                            260.2471, 0.005995147, \
                            (24622/6378) };

// functions

int main()
{
    FLOAT d;
    cartesian_coordinates_t theSun, thePlanet;

    printf("\nGeocentric Coordinates\n\n");

    for (d = 7855.0; d < 7865.0; d+= 0.25)
    {
        theSun.day = d;
        sunEclipticCartesianCoordinates ( &theSun);
        FPRINTF("%10s x %10.6f y %10.6f z %10.6f\n", sun.name, (float)theSun.x, (float)theSun.y, (float)theSun.z);

        thePlanet.day = d;
        planetEclipticCartesianCoordinates( &thePlanet, &moon );
        FPRINTF("%10s x %10.6f y %10.6f z %10.6f\n", moon.name, (float)thePlanet.x, (float)thePlanet.y, (float)thePlanet.z);

        planetEclipticCartesianCoordinates( &thePlanet, &mercury );
        addCartesianCoordinates( &thePlanet, &theSun );
        FPRINTF("%10s x %10.6f y %10.6f z %10.6f\n", mercury.name, (float)thePlanet.x, (float)thePlanet.y, (float)thePlanet.z);

        planetEclipticCartesianCoordinates( &thePlanet, &venus );
        addCartesianCoordinates( &thePlanet, &theSun );
        FPRINTF("%10s x %10.6f y %10.6f z %10.6f\n", venus.name, (float)thePlanet.x, (float)thePlanet.y, (float)thePlanet.z);

        planetEclipticCartesianCoordinates( &thePlanet, &mars );
        addCartesianCoordinates( &thePlanet, &theSun );
        FPRINTF("%10s x %10.6f y %10.6f z %10.6f\n", mars.name, (float)thePlanet.x, (float)thePlanet.y, (float)thePlanet.z);

        planetEclipticCartesianCoordinates( &thePlanet, &jupiter );
        addCartesianCoordinates( &thePlanet, &theSun );
        FPRINTF("%10s x %10.6f y %10.6f z %10.6f\n", jupiter.name, (float)thePlanet.x, (float)thePlanet.y, (float)thePlanet.z);

        planetEclipticCartesianCoordinates( &thePlanet, &saturn );
        addCartesianCoordinates( &thePlanet, &theSun );
        FPRINTF("%10s x %10.6f y %10.6f z %10.6f\n", saturn.name, (float)thePlanet.x, (float)thePlanet.y, (float)thePlanet.z);

        planetEclipticCartesianCoordinates( &thePlanet, &uranus );
        addCartesianCoordinates( &thePlanet, &theSun );
        FPRINTF("%10s x %10.6f y %10.6f z %10.6f\n", uranus.name, (float)thePlanet.x, (float)thePlanet.y, (float)thePlanet.z);

        planetEclipticCartesianCoordinates( &thePlanet, &neptune );
        addCartesianCoordinates( &thePlanet, &theSun );
        FPRINTF("%10s x %10.6f y %10.6f z %10.6f\n\n", neptune.name, (float)thePlanet.x, (float)thePlanet.y, (float)thePlanet.z);
    }
    printf("\nEnd\n");
    return 0;
}

void sunEclipticCartesianCoordinates ( cartesian_coordinates_t * sun) __z88dk_fastcall
{
    // We use formulas for finding the Sun as seen from Earth, 
    // then negate the (x,y,z) coordinates obtained to get the Earth's position 
    // from the Sun's perspective.

    // http://www.astro.uio.no/~bgranslo/aares/calculate.html
    // http://www.meteorobs.org/maillist/msg09197.html              <== Correct formulas, more accurate (complex)

    // These formulas use 'd' based on days since 1/Jan/2000 12:00 UTC ("J2000.0"), instead of 0/Jan/2000 0:00 UTC ("day value").
    // Correct by subtracting 1.5 days...
    FLOAT T = (sun->day - 1.5) * 0.0000273785;                      // 36525.0 Julian centuries since J2000.0

    FLOAT T_SQR = SQR(T);

    FLOAT L0 = rev(280.46645 + (36000.76983 * T) + (0.0003032 * T_SQR));                            // Sun's mean longitude, in degrees
    FLOAT M0 = rev(357.52910 + (35999.05030 * T) - (0.0001559 * T_SQR) - (0.00000048 * T * T_SQR));     // Sun's mean anomaly, in degrees

                                                                    // Sun's equation of center in degrees
    FLOAT C = rev((1.914600 - 0.004817 * T - 0.000014 * T_SQR) * SIN(RAD(M0)) + (0.01993 - 0.000101 * T) * SIN(RAD(2*M0)) + 0.000290 * SIN(RAD(3*M0)));

    FLOAT LS = rev(L0 + C);                                         // true ecliptical longitude of Sun

    FLOAT e = 0.016708617 - T * (0.000042037 + T * 0.0000001236);   // The eccentricity of the Earth's orbit.
    FLOAT distanceInAU = (1.000001018 * (1 - SQR(e))) / (1 + e * COS(RAD(M0 + C))); // distance from Sun to Earth in astronomical units (AU)
    sun->x = distanceInAU * COS(RAD(LS));
    sun->y = distanceInAU * SIN(RAD(LS));
    sun->z = 0.0;                                                   // the Earth's center is always on the plane of the ecliptic (z=0), by definition!
}


void planetEclipticCartesianCoordinates ( cartesian_coordinates_t * location, const planet_t * planet ) __z88dk_callee
{
    FLOAT day = location->day;

    FLOAT N = rev( planet->N0 + (day * planet->Nc) );
    FLOAT i = rev( planet->i0 + (day * planet->ic) );
    FLOAT w = rev( planet->w0 + (day * planet->wc) );
    FLOAT a = rev( planet->a0 + (day * planet->ac) );

    FLOAT e = rev( planet->e0 + (day * planet->ec) );
    FLOAT M = rev( planet->M0 + (day * planet->Mc) );

    FLOAT E = rev(eccentricAnomaly (e, M));

    // Calculate the body's position in its own orbital plane, and its distance from the thing it is orbiting.
    FLOAT xv = a * (COS(RAD(E)) - e);
    FLOAT yv = a * SQRT(1.0 - SQR(e)) * SIN(RAD(E));

    FLOAT v = DEG(ATAN2(yv, xv));       // True anomaly in degrees: the angle from perihelion of the body as seen by the Sun.
    FLOAT r = HYPOT(xv, yv);            // Distance from the Sun to the planet in AU

    FLOAT cosN  = COS(RAD(N));
    FLOAT sinN  = SIN(RAD(N));
    FLOAT cosi  = COS(RAD(i));
    FLOAT sini  = SIN(RAD(i));
    FLOAT cosVW = COS(RAD(v+w));
    FLOAT sinVW = SIN(RAD(v+w));

    // Now we are ready to calculate (unperturbed) ecliptic cartesian heliocentric coordinates.
    location->x = r * (cosN*cosVW - sinN*sinVW*cosi);
    location->y = r * (sinN*cosVW + cosN*sinVW*cosi);
    location->z = r * sinVW * sini;
}


FLOAT eccentricAnomaly (FLOAT e, FLOAT M) __z88dk_callee
{
    FLOAT E, error;

    E = M + (e * SIN(RAD(M)) * (1.0 + (e * COS(RAD(M)))));

    do {
        error = (E - DEG(e * SIN(RAD(E))) - M) / (1 - e * COS(RAD(E)));
        E -= error;
        error = FABS(error);
    } while (error >= 1.0e-3);          // the angle is good enough for our purposes

    return E;
}

void addCartesianCoordinates ( cartesian_coordinates_t * base, cartesian_coordinates_t * addend ) __z88dk_callee
{
    base->x += addend->x;
    base->y += addend->y;
    base->z += addend->z;
}


FLOAT rev (FLOAT x) __z88dk_fastcall
{
    return x - FLOOR(x*(1/360.0))*360.0;
}



