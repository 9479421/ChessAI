#pragma once

#include<iostream>
#include<opencv2/opencv.hpp>

#pragma comment(lib,"opencv_world450.lib")


struct Output {
	int id = -1;             //������id
	float confidence;   //������Ŷ�
	cv::Rect box;       //���ο�
	std::string name;  //��������
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
	const int netWidth = 640;   //ONNXͼƬ������
	const int netHeight = 640;  //ONNXͼƬ����߶�
	const int strideSize = 3;   //stride size
	const float netStride[4] = { 8, 16.0,32,64 };
	float boxThreshold = 0.35;
	float classThreshold = 0.35;   //���Ŷ�
	float nmsThreshold = 0.45;
	float nmsScoreThreshold = boxThreshold * classThreshold;
	std::vector<std::string> className = { "red_che", "red_ma", "red_xiang", "red_shi", "red_shuai", "red_pao", "red_bing", "black_che", "black_ma", "black_xiang",
	"black_shi", "black_jiang", "black_pao", "black_zu","board"};


	//	std::vector<std::string> className = { "�쳵", "����", "����", "����", "��˧", "����", "���", "�ڳ�", "����", "����",
	//"��ʿ", "�ڽ�", "����", "����" };
	//std::vector<std::string> className = { "��", "��", "��", "��", "˧", "��", "��", "��", "��", "��","ʿ", "��", "��", "��" };
	//std::vector<std::string> className = { "R", "N", "B", "A", "K", "C", "P", "r", "n", "b","a", "k", "c", "p" };
public:

};