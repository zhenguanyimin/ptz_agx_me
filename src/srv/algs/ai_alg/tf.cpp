// Test.cpp : Defines the entry point for the DLL application.
//
#include <iostream>
#include <opencv2/opencv.hpp>
#include <fstream>
#include <ctime>
#include <cmath>


double degree2radian(double degree)
{
	return degree * CV_PI / 180.0;
}

double radian2degree(double radian)
{
	return radian * 180.0 / CV_PI;
}

double distanceBetween(double startLatitude, double startLongitude, double endLatitude, double endLongitude)
{
	const double EARTH_RADIUS = 6378137.0;
	double dLat = degree2radian(endLatitude - startLatitude);
	double dLon = degree2radian(endLongitude - startLongitude);

	double a = pow(sin(dLat / 2), 2) + pow(sin(dLon / 2), 2) * cos(degree2radian(startLatitude)) * cos(degree2radian(endLatitude));
	double c = 2 * atan2(sqrt(a), sqrt(1 - a));

	return EARTH_RADIUS * c;
}

double bearingBetween(double startLatitude, double startLongitude, double endLatitude, double endLongitude)
{
	double startLongitudeRadian = degree2radian(startLongitude);
	double endLongitudeRadian = degree2radian(endLongitude);
	double startLatitudeRadian = degree2radian(startLatitude);
	double endLatitudeRadian = degree2radian(endLatitude);

	double y = sin(endLongitudeRadian - startLongitudeRadian) * cos(endLatitudeRadian);
	double x = cos(startLatitudeRadian) * sin(endLatitudeRadian) - sin(startLatitudeRadian) * cos(endLatitudeRadian) * cos(endLongitudeRadian - startLongitudeRadian);

	return radian2degree(atan2(y, x));
}

void calculateCoordinate(double *startLatLng, double *endLatLng, double high, double *out, double *angle)
{
	double distance = distanceBetween(startLatLng[0], startLatLng[1], endLatLng[0], endLatLng[1]);
	double bearing = bearingBetween(startLatLng[0], startLatLng[1], endLatLng[0], endLatLng[1]);

	double x = distance * cos(degree2radian(bearing));
	double y = distance * sin(degree2radian(bearing));

	if (abs(x) < 0.01)
		x = 0;
	if (abs(y) < 0.01)
		y = 0;

	out[0] = x;
	out[1] = y;
	out[2] = high; //z

	angle[0] = radian2degree(atan2(out[1], out[0]));
	angle[1] = radian2degree(atan2(out[2], sqrt(out[0]*out[0]+out[1]*out[1])));
}

// void CalibPTZ0(float wd, float jd, float gd, double* coordinate, double* angle)
// {
// 	double startLatLng[2] = { wd, jd };	//无人机
// 	double endLatLng[2] = { 0, 0 }; //ptz
// 	double coordinate[3] = { 0, 0, 0 };
// 	double angle[2] = { 0, 0 };
// 	calculateCoordinate(startLatLng, endLatLng, gd, coordinate, angle);

// 	double out[3]={0,0,0};
// 	out[0] = 393;
// 	out[1] = 59;
// 	double high=66;
// 	out[2] = sqrt(out[0]*out[0]+out[1]*out[1]+high*high);

// 	angle[0] = radian2degree(atan2(out[1], out[0]));
// 	angle[1] = radian2degree(asin(high/out[2]));
// 	std::cout<< "angle[0]:"<<angle[0]<<","<<"angle[1]:"<<angle[1]<<std::endl;
// }

void calibPTZ(double startLat, double startLng,double endLat,double endLng,double high,double* res)
{
	double startLatLng[2]={startLat, startLng};
	double endLatLng[2]={endLat, endLng};
	double out[3] = {0, 0, 0};
	double angle[2] = {0, 0};

	calculateCoordinate(startLatLng, endLatLng, high, out, angle);
	std::cout << "angle[0]:" << angle[0] << "," << "angle[1]:" << angle[1] << std::endl;

	res[0]=out[0];//x
	res[1]=out[1];//y
	res[2]=out[2];//z
	res[3]=angle[0];//a
	res[4]=angle[1];//e
	res[5]=sqrt(out[0]*out[0]+out[1]*out[1]+high*high); //range
}