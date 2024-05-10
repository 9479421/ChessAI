#pragma once

#include <onnxruntime_cxx_api.h>
#pragma comment (lib,"onnxruntime.lib")

#include <afx.h>
#include<stdarg.h>

#include <GdiPlus.h>
#pragma comment(lib, "Gdiplus.lib")

#include<iostream>
#include<vector>

typedef struct OutResult {
    int id = -1;
    int x1;
    int x2;
    int y1;
    int y2;
    
    float accuracy;

    OutResult() {

    }
    OutResult(int x1_, int x2_, int y1_, int y2_, int id_, float accuracy_) {
        x1 = x1_;
        x2 = x2_;
        y1 = y1_;
        y2 = y2_;
        id = id_;
        accuracy = accuracy_;
    }
} ;

class yolov7_OnnxRuntime
{
public:
    yolov7_OnnxRuntime();
    ~yolov7_OnnxRuntime();
	bool readModel(std::string onnxPath ,int classNums, ...);
	std::vector<OutResult> Detect(float minConfidence, std::string fileName);
    std::vector<OutResult> Detect(float minConfidence, HBITMAP hBitmap);
private:
    std::string img2Float(Gdiplus::Bitmap* bitmap); //ͼƬתfloat
    std::vector<OutResult> Detect(Gdiplus::Bitmap* orginalBitmap, float minConfidence);
private:
    ULONG_PTR m_gdiplusToken;
	std::vector<std::string> m_classNames;
    Ort::AllocatorWithDefaultOptions *m_allocator;
    Ort::SessionOptions *m_session_options;
    Ort::Env *m_env;
    Ort::Session *m_session;
    size_t m_num_input_nodes;
    size_t m_num_output_nodes;
    std::vector<char*> m_input_node_names;
    std::vector<char*> m_output_node_names;
    std::vector<int64_t> m_inputDims;
    int m_model_input_width;
    int m_model_input_height;
    int m_resizedWidth;
    int m_resizedHeight;
    int m_offsetX;
    int m_offsetY;

};

