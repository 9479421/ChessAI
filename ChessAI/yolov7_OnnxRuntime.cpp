#include "yolov7_Onnxruntime.h"


yolov7_OnnxRuntime::yolov7_OnnxRuntime()
{
    //初始化Gdiplus
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    Gdiplus::GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);
}

yolov7_OnnxRuntime::~yolov7_OnnxRuntime()
{
    delete m_session;
    delete m_allocator;
    delete m_session_options;
    delete m_env;
    for (auto ptr : m_input_node_names)
        m_allocator->Free(ptr);
    for (auto ptr : m_output_node_names)
        m_allocator->Free(ptr);
    Gdiplus::GdiplusShutdown(m_gdiplusToken);
}

bool yolov7_OnnxRuntime::readModel(std::string onnxPath, int classNums,...)
{
    try
    {
        //初始化className
        va_list vaList;
        va_start(vaList, classNums);
        for (size_t i = 0; i < classNums; i++)
        {
            char* className = va_arg(vaList, char*);
            m_classNames.push_back(className);
        }
        va_end(vaList);

        //初始化ONNXRuntime日志级别
        m_env = new Ort::Env(ORT_LOGGING_LEVEL_WARNING, "ONNXRuntime");
        //初始化OnnxRuntime
        m_session_options = new Ort::SessionOptions();
        m_allocator = new Ort::AllocatorWithDefaultOptions();
        m_session = new Ort::Session(*m_env, CA2W(onnxPath.c_str()), *m_session_options);

        m_num_input_nodes = m_session->GetInputCount();
        m_num_output_nodes = m_session->GetOutputCount();
        for (size_t i = 0; i < m_num_input_nodes; ++i)
        {
            m_input_node_names.push_back(m_session->GetInputName(i, *m_allocator));
        }
        for (size_t i = 0; i < m_num_output_nodes; ++i)
        {
            m_output_node_names.push_back(m_session->GetOutputName(i, *m_allocator));
        }
        //=============================================================================
        for (auto input_name : m_input_node_names)
        {
            std::cout << "input node name   : " << input_name << std::endl;
        }
        for (const char* output_name : m_output_node_names)
        {
            std::cout << "output node name  : " << output_name << std::endl;
        }
        std::cout << std::endl;
        //===============================================================================

        //读取模型文件大小
        m_inputDims = m_session->GetInputTypeInfo(0).GetTensorTypeAndShapeInfo().GetShape();
        m_model_input_width = m_inputDims.at(2);
        m_model_input_height = m_inputDims.at(3);

    }
    catch (const std::exception&) //捕获yolo加载异常
    {
        return false;
    }
   

    return true;
}


std::vector<OutResult> yolov7_OnnxRuntime::Detect(float minConfidence,std::string fileName)
{
    Gdiplus::Bitmap* bitmap = Gdiplus::Bitmap::FromFile(CA2W(fileName.c_str()));
    std::vector<OutResult> resultVector = Detect(bitmap, minConfidence);
    delete bitmap;
    return resultVector;
}

std::vector<OutResult> yolov7_OnnxRuntime::Detect(float minConfidence, HBITMAP hBitmap)
{
    Gdiplus::Bitmap* bitmap = Gdiplus::Bitmap::FromHBITMAP(hBitmap,NULL);
    std::vector<OutResult> resultVector = Detect(bitmap, minConfidence);
    delete bitmap;
    return resultVector;
}

std::string yolov7_OnnxRuntime::img2Float(Gdiplus::Bitmap* bitmap)
{
    int cols = bitmap->GetWidth();
    int rows = bitmap->GetHeight();

    double scale;
    if (cols / static_cast<double>(rows) > m_model_input_width / static_cast<double>(m_model_input_height))
    {
        scale = m_model_input_width / static_cast<double>(cols);
    }
    else {
        scale = m_model_input_height / static_cast<double>(rows);
    }
    m_resizedWidth = cols * scale;
    m_resizedHeight = rows * scale;

    Gdiplus::Bitmap* resizedBitmap = new Gdiplus::Bitmap(cols * scale, rows * scale, PixelFormat24bppRGB);
    Gdiplus::Graphics graphics(resizedBitmap);
    graphics.DrawImage(bitmap, Gdiplus::Rect(0, 0, cols * scale, rows * scale));

    //绘制到中间
    Gdiplus::Bitmap paddingBitmap(m_model_input_width, m_model_input_height, PixelFormat24bppRGB);
    m_offsetX = (int)(paddingBitmap.GetWidth() - m_resizedWidth) / 2;
    m_offsetY = (int)(paddingBitmap.GetHeight() - m_resizedHeight) / 2;
    Gdiplus::Graphics graphics_padding(&paddingBitmap);
    graphics_padding.DrawImage(resizedBitmap, Gdiplus::Rect(m_offsetX, m_offsetY, resizedBitmap->GetWidth(), resizedBitmap->GetHeight()));

    float* arr = (float*)malloc(3 * m_model_input_width * m_model_input_height * sizeof(float));
    for (size_t i = 0; i < m_model_input_height; i++)
    {
        for (size_t j = 0; j < m_model_input_width; j++)
        {
            if (i >= m_offsetY && j >= m_offsetX && i < m_offsetY + m_resizedHeight
                && j < m_offsetX + m_resizedWidth) {

                Gdiplus::Color color;
                resizedBitmap->GetPixel(j - m_offsetX, i - m_offsetY, &color);

                arr[0 * m_model_input_width * m_model_input_height + i * m_model_input_height + j] = color.GetRed() / 255.0f;
                arr[1 * m_model_input_width * m_model_input_height + i * m_model_input_height + j] = color.GetGreen() / 255.0f;
                arr[2 * m_model_input_width * m_model_input_height + i * m_model_input_height + j] = color.GetBlue() / 255.0f;
            }
            else {
                arr[0 * m_model_input_width * m_model_input_height + i * m_model_input_height + j] = 114.0f / 255;
                arr[1 * m_model_input_width * m_model_input_height + i * m_model_input_height + j] = 114.0f / 255;
                arr[2 * m_model_input_width * m_model_input_height + i * m_model_input_height + j] = 114.0f / 255;
            }
        }
    }
    std::string ret;
    ret.append((char*)arr, 3 * m_model_input_width * m_model_input_height * sizeof(float));


    delete resizedBitmap;
    delete arr;

    return ret;
}

std::vector<OutResult> yolov7_OnnxRuntime::Detect(Gdiplus::Bitmap* orginalBitmap,float minConfidence)
{
    //转成RGB24
    Gdiplus::Bitmap* newBitmap = orginalBitmap->Clone(Gdiplus::Rect(0, 0, orginalBitmap->GetWidth(), orginalBitmap->GetHeight()), PixelFormat24bppRGB);
    std::string floatData = img2Float(newBitmap);

    //开始识别图片
    Ort::MemoryInfo memoryInfo = Ort::MemoryInfo::CreateCpu(
        OrtAllocatorType::OrtArenaAllocator, OrtMemType::OrtMemTypeDefault);
    Ort::Value inputTensor = Ort::Value::CreateTensor<float>(memoryInfo,
        (float*)floatData.data(),
        floatData.size(),
        m_inputDims.data(),
        m_inputDims.size());
    std::vector<Ort::Value> outputTensors = m_session->Run(Ort::RunOptions{ nullptr },
        m_input_node_names.data(),
        &inputTensor,
        m_num_input_nodes,
        m_output_node_names.data(),
        m_num_output_nodes);
    auto* rawOutput = outputTensors[0].GetTensorData<float>();
    std::vector<int64_t> outputShape = outputTensors[0].GetTensorTypeAndShapeInfo().GetShape();
    size_t count = outputTensors[0].GetTensorTypeAndShapeInfo().GetElementCount();
    std::vector<float> output(rawOutput, rawOutput + count);

    std::vector<OutResult> resultVector;
    for (int i = 0; i < outputShape[0]; i++) {
        float x1 = output[i * outputShape[1] + 1];
        float y1 = output[i * outputShape[1] + 2];
        float x2 = output[i * outputShape[1] + 3];
        float y2 = output[i * outputShape[1] + 4];
        int classPrediction = output[i * outputShape[1] + 5];
        float accuracy = output[i * outputShape[1] + 6];
        // Coords should be scaled to the original image. The coords from the model are relative to the model's input height and width.
        x1 = ((x1 - m_offsetX) / m_resizedWidth) * orginalBitmap->GetWidth();
        x2 = ((x2 - m_offsetX) / m_resizedWidth) * orginalBitmap->GetWidth();
        y1 = ((y1 - m_offsetY) / m_resizedHeight) * orginalBitmap->GetHeight();
        y2 = ((y2 - m_offsetY) / m_resizedHeight) * orginalBitmap->GetHeight();
        OutResult result(x1, x2, y1, y2, classPrediction, accuracy);
        resultVector.push_back(result);
    }

    //输出结果
    for (auto result : resultVector) {
        if (result.accuracy > minConfidence) {
            printf("x1:%d y1:%d x2:%d y2:%d id:%d name:%s confidience:%f\n", result.x1, result.y1, result.x2, result.y2, result.id, m_classNames[result.id], result.accuracy);
        }
    }
    delete newBitmap;
    
    return resultVector;
}
