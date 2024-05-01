#pragma once

#include<iostream>
#include<opencv2/opencv.hpp>

#pragma comment(lib,"opencv_world450.lib")


struct Output {
	int id = -1;             //结果类别id
	float confidence;   //结果置信度
	cv::Rect box;       //矩形框
	std::string name;  //棋子名字
};


class Yolo {
public:
	Yolo() {
	}
	~Yolo() {}
	bool readModel(cv::dnn::Net& net, std::string& netPath, bool isCuda);
	bool Detect(cv::Mat& SrcImg, cv::dnn::Net& net, std::vector<Output>& output);

private:
	float sigmoid_x(float x)
	{
		return static_cast<float>(1.f / (1.f + exp(-x)));
	}
	const float netAnchors[3][6] = { {12, 16, 19, 36, 40, 28},{36, 75, 76, 55, 72, 146},{142, 110, 192, 243, 459, 401} }; //yolov7-P5 anchors
	const int netWidth = 640;   //ONNX图片输入宽度
	const int netHeight = 640;  //ONNX图片输入高度
	const int strideSize = 3;   //stride size
	const float netStride[4] = { 8, 16.0,32,64 };
	float boxThreshold = 0.35;
	float classThreshold = 0.35;   //置信度
	float nmsThreshold = 0.45;
	float nmsScoreThreshold = boxThreshold * classThreshold;
	std::vector<std::string> className = { "red_che", "red_ma", "red_xiang", "red_shi", "red_shuai", "red_pao", "red_bing", "black_che", "black_ma", "black_xiang",
	"black_shi", "black_jiang", "black_pao", "black_zu","board"};


	//	std::vector<std::string> className = { "红车", "红马", "红相", "红仕", "红帅", "红炮", "红兵", "黑车", "黑马", "黑象",
	//"黑士", "黑将", "黑炮", "黑卒" };
	//std::vector<std::string> className = { "车", "马", "相", "仕", "帅", "炮", "兵", "车", "马", "象","士", "将", "炮", "卒" };
	//std::vector<std::string> className = { "R", "N", "B", "A", "K", "C", "P", "r", "n", "b","a", "k", "c", "p" };
public:

};