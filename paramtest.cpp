// non sweep method for testing parameters
//

#include "stdafx.h"
#include "util.h"
#include "../DepthSensorAdaptor/DepthSensorAdaptor.h"
#include "../DepthSensorAdaptor/depth_map.h"
#include "../common/include/test_depth_accuracy.h"
#include "../common/include/point_cloud_data.h"
#include <iostream>
using namespace std;
// #define ACCURACY_TEST
int _mode = RealSense_D415;

void testCamerParams(DepthSensor* pDepthSensor) {
	map<int, cv::Mat_<float>> params = pDepthSensor->getCameraParams();

	if (params.find(rgb_intrinsic_3_3) != params.end()) {
		std::cout << "rgb intrinsic " << std::endl;
		std::cout << params[rgb_intrinsic_3_3] << std::endl;
	}
	if (params.find(rgb_distortion_1_5) != params.end()) {
		std::cout << "rgb distortion " << std::endl;
		std::cout << params[rgb_distortion_1_5] << std::endl;
	}
	if (params.find(depth_intrinsic_3_3) != params.end()) {
		std::cout << "depth intrinsic " << std::endl;
		std::cout << params[depth_intrinsic_3_3] << std::endl;
	}
	if (params.find(depth_distortion_1_5) != params.end()) {
		std::cout << "depth distortion " << std::endl;
		std::cout << params[depth_distortion_1_5] << std::endl;
	}
	if (params.find(from_ir_to_rgb_extrinsic_4_4) != params.end()) {
		std::cout << "ir to rgb extrinsic " << std::endl;
		std::cout << params[from_ir_to_rgb_extrinsic_4_4] << std::endl;
	} 
	if (params.find(from_depth_to_rgb_extrinsic_4_4) != params.end()) {
		std::cout << "depth to rgb extrinsic " << std::endl;
		std::cout << params[from_depth_to_rgb_extrinsic_4_4] << std::endl;
	}
	std::cout << "the depth scale is " << pDepthSensor->getDepthScale() << std::endl;
}

int main()
{
	
	//std::cout << "main called" << std::endl;
	std::cout << "Please choose sensor type: " << std::endl;
	std::cout << "1. RealSense SR300" << std::endl;
	std::cout << "2. Orbec Astra" << std::endl;
	std::cout << "3. Tuyang FM810" << std::endl;
	std::cout << "4. RealSense D415" << std::endl;
	cin >> _mode;
	if (_mode < 0 || _mode > SensorTypeLengh) {
		return 1;
	}
	_mode = _mode - 1;
	DepthSensor* pDepthSensor = NULL;
	pDepthSensor = DepthSensor::getSensor((SensorType)_mode);

	/*
	if (Orbec_Atras == _mode) {
		pDepthSensor = DepthSensor::getSensor(Orbec_Atras);
	}
	else if (Tuyang_810 == _mode) {
		pDepthSensor = DepthSensor::getSensor(Tuyang_810);
	} */
	
	pDepthSensor->init();
	pDepthSensor->enableStream(rgb_stream);
	pDepthSensor->enableStream(depth_stream);
	//pDepthSensor->enableStream(ir_stream);
	if (Orbec_Atras != _mode) {
		pDepthSensor->enableStream(ir_stream);
	}
	testCamerParams(pDepthSensor);
	cv::Mat rgbMat, depthMat, irMat;
	DepthSensorUtil::DepthRender depthRender;
	depthRender.SetRangeMode(DepthSensorUtil::DepthRender::COLOR_RANGE_ABS);
	depthRender.SetColorType(DepthSensorUtil::DepthRender::COLORTYPE_RAINBOW);
	int near = 200, far = 1000;
	float scale = pDepthSensor->getDepthScale();
	if (0 != scale) {
		near = int(((float)near / 1000) / scale);
		far = int(((float)far / 1000) / scale);
	}
	depthRender.SetColorRange(near, far);
	Timer timer;
	while (true) {
		timer.tic();
		int keyCode = cv::waitKey(1);
		if (27 == keyCode) {
			break;
		}
		bool aligned = true;
#ifdef ACCURACY_TEST
		aligned = true;
#endif

		pDepthSensor->cap(rgbMat, depthMat, irMat, false);
#ifdef ACCURACY_TEST
		if (!irMat.empty() && !depthMat.empty()) {
			map<int, cv::Mat_<float>> params = pDepthSensor->getCameraParams();
			if (params.find(depth_intrinsic_3_3) != params.end()) {
				cv::Mat_<float> intrinsic = params[depth_intrinsic_3_3];
				float fx = intrinsic(0, 0);
				float fy = intrinsic(1, 1);
				float error, variant;
				cv::cvtColor(rgbMat, irMat, CV_RGB2GRAY);
				int result = testDepthAccuracy(irMat, (uint16_t *)depthMat.ptr(0), 640, 480, pDepthSensor->getDepthScale(), fx, fy, error, variant);
				if (result) {
					std::cout << "error " << error << " variant" << variant << std::endl;
					char *text = new char[2048];
					sprintf(text, "variant: %1.1f error: %1.1f", variant * 1000, error * 1000);
					putText(rgbMat, text, Point(10, 20), FONT_HERSHEY_SIMPLEX, 0.6, Scalar(255, 0, 0), 1);
				}
			}

		}
#endif
		if (rgbMat.rows > 0) {

			cv::Mat bgrMat;
			cv::cvtColor(rgbMat, bgrMat, CV_RGB2BGR);
			cv::imshow("Color", bgrMat);
		}
		if (depthMat.rows > 0) {
			cv::Mat colorDepth = depthRender.Compute(depthMat);
			cv::imshow("Depth", colorDepth);
		}
		if (irMat.rows > 0) {
			cv::imshow("ir", irMat);
		}

		if (keyCode == 115) {
			//user press 's', just save depth img as ptx file
			pDepthSensor->cap(rgbMat, depthMat, irMat, false);
			std::vector<DepthSensorUtil::Point3D> points;
			map<int, cv::Mat_<float>> params = pDepthSensor->getCameraParams();
			if (params.find(depth_intrinsic_3_3) != params.end()) {
				DepthSensorUtil::depthImageToPointCloud(depthMat, points, params[depth_intrinsic_3_3], pDepthSensor->getDepthScale());
				DepthSensorUtil::savePointCloud(points);
				std::cout << "save the point cloud file" << std::endl;
			}
		}
		float fps = 1000 / timer.toc();
		cout << "the fps is " << fps << endl;
	}
	pDepthSensor->release();
	// cin.ignore();
	return 0;
}
