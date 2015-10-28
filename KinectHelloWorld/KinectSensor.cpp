#include "KinectSensor.h"

KinectSensor::KinectSensor()
{
	height = 480;
	width = 640;
	depthToRgbMap = new long[width*height*2];
}

KinectSensor::~KinectSensor()
{

}

bool KinectSensor::init()
{
	int numSensors;

	//stop if we don't find a sensor
	if (NuiGetSensorCount(&numSensors) < 0 || numSensors < 1)
	{
		return false;
	}

	//stop if we were not able to create the sensor
	//Assumes that the user wants to use the first kinect found
	//or that there is only one sensor connected
	if (NuiCreateSensorByIndex(0, &sensor) < 0)
	{
		return false;
	}

	//Initialize the sensor
	sensor->NuiInitialize(NUI_INITIALIZE_FLAG_USES_DEPTH | NUI_INITIALIZE_FLAG_USES_COLOR);
	sensor->NuiImageStreamOpen(
		NUI_IMAGE_TYPE_COLOR,			//get the color images
		NUI_IMAGE_RESOLUTION_640x480,	//Image resolution
		0,								//Image stream flags e.g near mode
		2,								//number of frames to buffer
		NULL,							//event handle
		&rgbStream);

	sensor->NuiImageStreamOpen(
		NUI_IMAGE_TYPE_DEPTH,			//Get the depth images
		NUI_IMAGE_RESOLUTION_640x480,	//Image resolution
		0,								//Image stream flags e.g near mode
		2,								//number of frames to buffer
		NULL,							//Event handle
		&depthStream);

	return true;
}

HANDLE KinectSensor::getDepthStream()
{
	return depthStream;
}

HANDLE KinectSensor::getRgbStream()
{
	return rgbStream;
}

INuiSensor * KinectSensor::getSensor()
{
	return sensor;
}

long * KinectSensor::getDepthToRgbMap()
{
	return depthToRgbMap;
}


void KinectSensor::getColorData(GLubyte * dest)
{
	float* fdest = (float*)dest;
	memset(fdest, 0, sizeof(dest));
	long* depth2rgb = (long*)depthToRgbMap;
	NUI_IMAGE_FRAME imageFrame;
	NUI_LOCKED_RECT LockedRect;
	if (sensor->NuiImageStreamGetNextFrame(rgbStream, 0, &imageFrame) < 0) return;
	INuiFrameTexture* texture = imageFrame.pFrameTexture;
	texture->LockRect(0, &LockedRect, NULL, 0);
	if (LockedRect.Pitch != 0) {
		const BYTE* start = (const BYTE*)LockedRect.pBits;
		for (int j = 0; j < height; ++j) {
			for (int i = 0; i < width; ++i) {
				// Determine rgb color for each depth pixel
				long x = *depth2rgb++;
				long y = *depth2rgb++;
				// If out of bounds, then don't color it at all
				if (x < 0 || y < 0 || x > width || y > height) {
					for (int n = 0; n < 3; ++n) *(fdest++) = 0.0f;
				}
				else {
					const BYTE* curr = start + (x + width*y) * 4;
					for (int n = 0; n < 3; ++n) *(fdest++) = curr[2 - n] / 255.0f;
				}

			}
		}
	}
	texture->UnlockRect(0);
	sensor->NuiImageStreamReleaseFrame(rgbStream, &imageFrame);
}

void KinectSensor::getDepthData(GLubyte * dest)
{
	float * fdest = (float *)dest;
	memset(fdest, 0, sizeof(dest));
	long * depth2Rgb = (long *)depthToRgbMap;

	NUI_IMAGE_FRAME imageFrame;
	NUI_LOCKED_RECT lockedRect;

	//if we can't get the image stop
	if (sensor->NuiImageStreamGetNextFrame(depthStream, 0, &imageFrame) < 0)
	{
		return;
	}

	INuiFrameTexture * texture = imageFrame.pFrameTexture;
	texture->LockRect(0, &lockedRect, NULL, 0);
	if (lockedRect.Pitch != 0)
	{
		const USHORT * curr = (const USHORT *)lockedRect.pBits;
		for (int j = 0; j < height; ++j)
		{
			for (int i = 0; i < width; ++i)
			{
				//get depth in millimeters
				USHORT depth = NuiDepthPixelToDepth(*curr++);

				//Store coords of the point
				Vector4 pos = NuiTransformDepthImageToSkeleton(i, j, depth << 3, NUI_IMAGE_RESOLUTION_640x480);
				*fdest++ = pos.x / pos.w;
				*fdest++ = pos.y / pos.w;
				*fdest++ = pos.z / pos.w;

				//store index in the color array
				NuiImageGetColorPixelCoordinatesFromDepthPixelAtResolution(
					NUI_IMAGE_RESOLUTION_640x480, NUI_IMAGE_RESOLUTION_640x480, NULL,
					i, j, depth << 3, depth2Rgb, depth2Rgb + 1
					);

				depth2Rgb += 2;
			}
		}
	}

	texture->UnlockRect(0);
	sensor->NuiImageStreamReleaseFrame(depthStream, &imageFrame);
}

void KinectSensor::move(LONG newAngle)
{
	LONG oldAngle = 0.0;
	NuiCameraElevationGetAngle(&oldAngle);
	NuiCameraElevationSetAngle(oldAngle + newAngle);
}