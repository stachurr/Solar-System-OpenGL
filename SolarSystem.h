#pragma once
#include <limits>

typedef float EarthRatio;
typedef GLuint DisplayList, Texture;

float fmod360(float f) {
	return (float)fmod( f, 360 );
}
float degToRad(float f) {
	return f * (float)M_PI / 180.0f;
}
float radToDeg(float f) {
	return f * 180.0f / (float)M_PI;
}

class Planet {
private:
	// create the texture
	void initTexture(char* texturePath) {
		int TexWidth, TexHeight;
		unsigned char* Pixels;

		Pixels = BmpToTexture(texturePath, &TexWidth, &TexHeight);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		glGenTextures(1, &(this->texture));
		glBindTexture(GL_TEXTURE_2D, this->texture);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, 3, TexWidth, TexHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, Pixels);
	}

	// create display lists
	void initDisplayLists() {
		int resolution = (int)(40 * SCALE);

		// default (to scale)
		this->defaultDL = glGenLists( 1 );
		glNewList(this->defaultDL, GL_COMPILE);
			glEnable(GL_TEXTURE_2D);
				OsuSphere(this->defaultRadius, resolution, resolution);
			glDisable(GL_TEXTURE_2D);
		glEndList();

		// adjusted
		this->adjustedDL = glGenLists( 1 );
		glNewList(this->adjustedDL, GL_COMPILE);
			glEnable(GL_TEXTURE_2D);
				OsuSphere(this->adjustedRadius, resolution, resolution);
			glDisable(GL_TEXTURE_2D);
		glEndList();

		// the current display list to use
		this->nowDL = this->adjustedDL;
	}

public:
	char*		name;
	float		axisTilt;
	EarthRatio	orbitalPeriod, orbitalRadius, defaultRadius, adjustedRadius, nowRadius, rotationalPeriod;
	DisplayList	nowDL, defaultDL, adjustedDL;
	Texture		texture;

	// Default constructor (needed for SolarSystem)
	Planet() {
		this->name		= "Unnamed";

		this->axisTilt		= 0.0f;

		this->orbitalPeriod	= 0.0f;
		this->orbitalRadius	= 0.0f;
		this->defaultRadius	= 0.0f;
		this->adjustedRadius	= 0.0f;
		this->nowRadius		= 0.0f;
		this->rotationalPeriod	= 0.0f;

		this->texture		= 0;

		this->defaultDL		= 0;
		this->adjustedDL	= 0;
		this->nowDL		= 0;
	}

	// Primary constructor, initialize with parameters
	Planet(
		char* name,
		float axisTilt,
		EarthRatio defaultRadius,
		EarthRatio adjustedRadius,
		EarthRatio orbitalRadius,
		EarthRatio rotationalPeriod,
		char* texturePath
	) {
		this->name		= name;

		this->axisTilt		= axisTilt;

		this->orbitalPeriod	= (float)pow(orbitalRadius, 3.0 / 2.0);
		this->orbitalRadius	= orbitalRadius * SCALE;
		this->defaultRadius	= defaultRadius * SCALE;
		this->adjustedRadius	= adjustedRadius * SCALE;
		this->nowRadius		= this->adjustedRadius;
		this->rotationalPeriod	= rotationalPeriod;

		this->initTexture(texturePath);

		this->initDisplayLists();
	}

	// change scale
	void updatePlanetScale( bool adjusted ) {
		this->nowDL	= adjusted ? this->adjustedDL : this->defaultDL;
		this->nowRadius	= adjusted ? this->adjustedRadius : this->defaultRadius;
	}
};
class SolarSystem {
private:
	static const int	NUM_BODIES			= 9;
	static const int	ANIMATION_STEP			= 5000000;
	static const int	ANIMATION_SPEED_MIN		= 100000;
	static const int	ANIMATION_SPEED_MAX		= 100000000;
	static const int	ANIMATION_SPEED_DEFAULT		= 10000000;
	static constexpr float	SEC_PER_YEAR			= 31536000;
	static constexpr float	EARTH_ROTATIONS_PER_YEAR	= 365.2422f;
	static constexpr float	SPACING				= 5.0f * SCALE;

	// SUN, MERCURY, VENUS, EARTH, MARS, JUPITER, SATURN, URANUS, NEPTUNE
	static constexpr EarthRatio	RADIUS[NUM_BODIES] = {
		25.0f,
		0.1915f,
		0.4745f,
		1.0f,
		0.266f,
		5.605f,
		4.725f,
		2.005f,
		1.94f
	};
	static constexpr EarthRatio	RADIUS_ADJUSTED[NUM_BODIES] = {
		RADIUS[0],
		RADIUS[1] * 10.0f,
		RADIUS[2] * 6.0f,
		RADIUS[3] * 3.0f,
		RADIUS[4] * 8.0f,
		RADIUS[5],
		RADIUS[6],
		RADIUS[7] * 1.5f,
		RADIUS[8] * 1.5f
	};
	static constexpr EarthRatio	ORBITAL_RADIUS[NUM_BODIES] = {
		std::numeric_limits<float>::infinity(),
		0.387f,
		0.723f,
		1.0f,
		1.52f,
		5.20f,
		9.58f,
		19.20f,
		30.05f
	};
	static constexpr EarthRatio	ROTATIONAL_PERIOD[NUM_BODIES] = {
		std::numeric_limits<float>::infinity(),
		58.8f,
		-244.0f,
		1.0f,
		1.03f,
		0.415f,
		0.445f,
		-0.720f,
		0.673f
	};
	static constexpr float		AXIS_TILT[NUM_BODIES] = {
		7.25f,
		0.01f,
		177.40f,
		23.44f,
		25.19f,
		3.13f,
		26.73f,
		97.77f,
		28.32f
	};
	static constexpr char*		BMP_PATHS[NUM_BODIES] = {
		"images/2k_sun.bmp",
		"images/2k_mercury.bmp",
		"images/2k_venus.bmp",
		"images/2k_earth_daymap.bmp",
		"images/2k_mars.bmp",
		"images/2k_jupiter.bmp",
		"images/2k_saturn.bmp",
		"images/2k_uranus.bmp",
		"images/2k_neptune.bmp"
	};
	static constexpr char*		NAME[NUM_BODIES] = {
		"Sol",
		"Mercury",
		"Venus",
		"Earth",
		"Mars",
		"Jupiter",
		"Saturn",
		"Uranus",
		"Neptune"
	};

	DisplayList animationSpeedButtonsDL;
	Planet bodies[NUM_BODIES];
	int animationSpeed;	// 2 means twice as fast, 3 is 3x as fast, etc.
	float earthYearCount, earthDayCount, earthOrbitDuration;

	// create display lists
	void initButtonDisplayList() {
		float x		= 18;
		float y		= 99;
		float width	= 2;
		float height	= 4;

		this->animationSpeedButtonsDL = glGenLists( 1 );
		glNewList(this->animationSpeedButtonsDL, GL_COMPILE);
			glColor3f(0.5, 0.5, 0.5);
			glBegin(GL_QUADS);
				glVertex3f(x, y, 0);
				glVertex3f(x, y - height, 0);
				glVertex3f(x + width, y - height, 0);
				glVertex3f(x + width, y, 0);
			glEnd();

			glColor3f(1, 1, 1);
			glBegin(GL_LINE_STRIP);
				glVertex3f(x, y, 0);
				glVertex3f(x, y - height, 0);
				glVertex3f(x + width, y - height, 0);
				glVertex3f(x + width, y, 0);
				glVertex3f(x, y, 0);
				glVertex3f(x, y - height / 2, 0);
				glVertex3f(x + width, y - height / 2, 0);
			glEnd();

			glBegin(GL_LINE_STRIP);
				glVertex3f(x + width / 4, y - 2 * height / 6, 0);
				glVertex3f(x + width / 2, y - height / 6, 0);
				glVertex3f(x + 3 * width / 4, y - 2 * height / 6, 0);
			glEnd();

			glBegin(GL_LINE_STRIP);
				glVertex3f(x + width / 4, y - 4 * height / 6, 0);
				glVertex3f(x + width / 2, y - 5 * height / 6, 0);
				glVertex3f(x + 3 * width / 4, y - 4 * height / 6, 0);
			glEnd();
		glEndList();
	}

	// creates the sun and planets object instances
	// NOTE*:	to be called after the graphics are initialized (so textures
	//		can be applied to bodies) and after Reset() is called
	void initBodies() {
		for (int i = 0; i < NUM_BODIES; i++)
			this->bodies[i] = Planet(
				NAME[i],
				AXIS_TILT[i],
				RADIUS[i],
				RADIUS_ADJUSTED[i],
				ORBITAL_RADIUS[i],
				ROTATIONAL_PERIOD[i],
				BMP_PATHS[i]
			);
	}

public:
	// default constructor
	SolarSystem() {
		animationSpeed		= ANIMATION_SPEED_DEFAULT;
		earthYearCount		= 0.0f;
		earthDayCount		= 0.0f;
		earthOrbitDuration	= SEC_PER_YEAR / (float)ANIMATION_SPEED_DEFAULT;

		animationSpeedButtonsDL = 0;
	}

	// getters
	DisplayList	getAnimationSpeedButtonsDL() {
		return this->animationSpeedButtonsDL;
	}
	Planet	getPlanet(int index) {
		return this->bodies[index];
	}
	int	getAnimationSpeed() {
		return this->animationSpeed;
	}
	float	getEarthYearCount() {
		return this->earthYearCount;
	}
	float	getEarthDayCount() {
		return this->earthDayCount;
	}
	float	getEarthOrbitDuration() {
		return this->earthOrbitDuration;
	}

	// call init functions
	void init() {
		this->initButtonDisplayList();
		this->initBodies();
	}

	// changes the scale ratios used without reloading textures
	void updatePlanetScales( bool adjusted ) {
		for (int i = 1; i < NUM_BODIES; i++) {
			this->bodies[i].updatePlanetScale( adjusted );
		}
	}

	// increments the animation speed by ANIMATION_STEP (5,000,000).
	// handles special case: ANIMATION_MIN (100,000).
	void increaseAnimationSpeed() {
		animationSpeed = ( animationSpeed == ANIMATION_SPEED_MIN )
			? ANIMATION_STEP
			: ( animationSpeed == ANIMATION_SPEED_MAX )
				? ANIMATION_SPEED_MAX
				: animationSpeed + ANIMATION_STEP;

		// update
		earthOrbitDuration = (float)SEC_PER_YEAR / (float)animationSpeed;
	}

	// decrements the animation speed by ANIMATION_STEP (5,000,000).
	// handles special case: ANIMATION_MIN (100,000).
	void decreaseAnimationSpeed() {
		animationSpeed = ( animationSpeed == ANIMATION_SPEED_MIN || animationSpeed == ANIMATION_STEP )
			? ANIMATION_SPEED_MIN
			: animationSpeed - ANIMATION_STEP;

		// update
		earthOrbitDuration = (float)SEC_PER_YEAR / (float)animationSpeed;
	}

	// draws the solar system
	void draw(float Time) {
		float dx	= -this->getPlanet(0).nowRadius;
		earthYearCount	= Time / earthOrbitDuration;
		earthDayCount	= earthYearCount * EARTH_ROTATIONS_PER_YEAR;

		// orbit paths
		if ( OrbitPathsOn ) {
			glDisable(GL_LIGHTING);
			glColor3fv(MulArray3(0.2f, (float*)White));
			for (int i = 0; i < NUM_BODIES; i++) {
				glBegin(GL_LINE_STRIP);
					Planet currentBody = this->bodies[i];
					float distanceFromSun = dx + currentBody.nowRadius;

					// use distance to draw circle
					float numSegs = 99;
					for (int j = 0; j <= numSegs; j++) {
						float p = (float)j / numSegs * 2 * (float)M_PI;
						float x = distanceFromSun * (float)cos(p);
						float z = distanceFromSun * (float)sin(p);
						glVertex3f(x, 0, z);
					}
				glEnd();
				dx += (2 * currentBody.nowRadius) + SPACING;
			}
			glColor3fv((float*)White);
			glEnable(GL_LIGHTING);
		}

		// draw each body
		dx = -this->getPlanet(0).nowRadius;
		for (int i = 0; i < NUM_BODIES; i++) {
			Planet currentBody	= this->bodies[i];
			float orbitAngleRad	= degToRad(	fmod360( earthYearCount * 360 / currentBody.orbitalPeriod ) );
			float rotationAngleDeg	= 		fmod360( earthDayCount * 360 / currentBody.rotationalPeriod );

			// bind current body's texture
			glBindTexture(GL_TEXTURE_2D, currentBody.texture);

			// draw current planet
			glPushMatrix();
				float distanceFromSun	= dx + currentBody.nowRadius;
				float x			= distanceFromSun * (float)cos(orbitAngleRad);
				float z			= distanceFromSun * (float)sin(orbitAngleRad);

				glTranslatef(x, 0, -z);				// orbit
				glRotatef(currentBody.axisTilt, 1, 0, 0);	// axis tilt
				glRotatef(rotationAngleDeg, 0, 1, 0);		// rotation

				// draw sun
				if (i == 0) {
					SetPointLight(GL_LIGHT0,	0, 0, 0,	1, 0.75, 0.5);	// sun light

					glDisable(GL_LIGHTING);
						glCallList(currentBody.nowDL);
					glEnable(GL_LIGHTING);
				}
				// draw planet
				else {
					glCallList(currentBody.nowDL);

					// rotational axis line
					if ( AxisOfRotationLineOn ) {
						glDisable(GL_LIGHTING);
							glColor3fv((float*)White);
							float axisLineRadius = currentBody.nowRadius + (3 * SCALE);

							glBegin(GL_LINES);
								glVertex3f(0, -axisLineRadius, 0);
								glVertex3f(0, axisLineRadius, 0);
							glEnd();
						glEnable(GL_LIGHTING);
					}
				}

			glPopMatrix();

			dx += (2 * currentBody.nowRadius) + SPACING;
		}
	}
};
