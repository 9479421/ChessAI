#include "pch.h"
#include "yolov7.h"

bool Yolo::readModel(cv::dnn::Net& net, std::string& netPath, bool isCuda)
{
	try {
		net = cv::dnn::readNet(netPath);
	}
	catch (const std::exception&) {
		return false;
	}

	//加速处理
	if (isCuda)
	{
		net.setPreferableBackend(cv::dnn::DNN_BACKEND_CUDA);
		net.setPreferableTarget(cv::dnn::DNN_TARGET_CUDA);
	}
	else
	{
		net.setPreferableBackend(cv::dnn::DNN_BACKEND_DEFAULT);
		net.setPreferableTarget(cv::dnn::DNN_TARGET_CPU);
	}
	return true;
}
#include"Utils.h"
bool Yolo::Detect(cv::Mat& SrcImg, cv::dnn::Net& net, std::vector<Output>& output) {

	cv::Mat blob;
	int col = SrcImg.cols;
	int row = SrcImg.rows;
	int maxLen = MAX(col, row);
	cv::Mat netInputImg = SrcImg.clone();
	if (maxLen > 1.2 * col || maxLen > 1.2 * row) {
		cv::Mat resizeImg = cv::Mat::zeros(maxLen, maxLen, CV_8UC3);
		SrcImg.copyTo(resizeImg(cv::Rect(0, 0, col, row)));
		netInputImg = resizeImg;

	}


	cv::dnn::blobFromImage(netInputImg, blob, 1 / 255.0, cv::Size(netWidth, netHeight), cv::Scalar(0, 0, 0), true, false);
	net.setInput(blob);
	std::vector<cv::Mat> netOutputImg(100);
	net.forward(netOutputImg, net.getUnconnectedOutLayersNames());
	std::vector<int> classIds(100);//结果id数组
	std::vector<float> confidences(100);//结果每个id对应置信度数组
	std::vector<cv::Rect> boxes(100);//每个id矩形框
	float ratio_h = (float)netInputImg.rows / netHeight;
	float ratio_w = (float)netInputImg.cols / netWidth;
	int net_width = className.size() + 5;  //输出的网络宽度是类别数+5
	for (int stride = 0; stride < strideSize; stride++) {    //stride
		float* pdata = (float*)netOutputImg[stride].data;
		int grid_x = (int)(netWidth / netStride[stride]);
		int grid_y = (int)(netHeight / netStride[stride]);
		for (int anchor = 0; anchor < 3; anchor++) {	//anchors
			const float anchor_w = netAnchors[stride][anchor * 2];
			const float anchor_h = netAnchors[stride][anchor * 2 + 1];
			for (int i = 0; i < grid_y; i++) {
				for (int j = 0; j < grid_x; j++) {
					float box_score = sigmoid_x(pdata[4]); ;//获取每一行的box框中含有某个物体的概率
					if (box_score >= boxThreshold) {
						cv::Mat scores(1, className.size(), CV_32FC1, pdata + 5);
						cv::Point classIdPoint;
						double max_class_socre;
						minMaxLoc(scores, 0, &max_class_socre, 0, &classIdPoint);
						max_class_socre = sigmoid_x(max_class_socre);
						if (max_class_socre >= classThreshold) {
							float x = (sigmoid_x(pdata[0]) * 2.f - 0.5f + j) * netStride[stride];  //x
							float y = (sigmoid_x(pdata[1]) * 2.f - 0.5f + i) * netStride[stride];   //y
							float w = powf(sigmoid_x(pdata[2]) * 2.f, 2.f) * anchor_w;   //w
							float h = powf(sigmoid_x(pdata[3]) * 2.f, 2.f) * anchor_h;  //h
							int left = (int)(x - 0.5 * w) * ratio_w + 0.5;
							int top = (int)(y - 0.5 * h) * ratio_h + 0.5;
							classIds.push_back(classIdPoint.x);
							confidences.push_back(max_class_socre * box_score);
							boxes.push_back(cv::Rect(left, top, int(w * ratio_w), int(h * ratio_h)));
						}
					}
					pdata += net_width;//下一行
				}
			}
		}
	}
	//执行非最大抑制以消除具有较低置信度的冗余重叠框（NMS）
	std::vector<int> nms_result(100);
	cv::dnn::NMSBoxes(boxes, confidences, nmsScoreThreshold, nmsThreshold, nms_result);
	for (int i = 0; i < nms_result.size(); i++) {
		int idx = nms_result[i];
		Output result;
		result.id = classIds[idx];
		result.confidence = confidences[idx];
		result.box = boxes[idx];
		/*result.name = className[result.id];*/
		if (confidences[idx] > 0.8)
		{
			output.push_back(result);
		}
	}

	for (int i = 0; i < output.size(); i++) {
		std::cout << output[i].id << " " << output[i].confidence << " " << output[i].box.x << " " << output[i].box.y << " " << output[i].box.width << " " << output[i].box.height << std::endl;
	}

	if (output.size())
		return true;
	else
		return false;
}
