#include <iostream>

#include <node.h>
#include <opencv2/core.hpp> // cvRound
#include <opencv2/objdetect.hpp>
#include <opencv2/highgui.hpp> // imread
#include <opencv2/imgproc.hpp> // cvtColor
#include <opencv2/cudaobjdetect.hpp>
#include <opencv2/cudaimgproc.hpp>

const static std::vector<int> imwriteargs = {cv::IMWRITE_JPEG_QUALITY, 95};

static cv::CascadeClassifier face_cascade;
static cv::CascadeClassifier eyes_cascade;
static cv::Ptr<cv::cuda::CascadeClassifier> cascade_gpu;

static bool bIsCuda = false;

void LoadCascades(const v8::FunctionCallbackInfo<v8::Value>& args) {
  v8::String::Utf8Value mod_path(args[0]->ToString());
  std::string mod_str = std::string(*mod_path);
  bIsCuda = cv::cuda::getCudaEnabledDeviceCount();
  if (bIsCuda) {
    cascade_gpu = cv::cuda::CascadeClassifier::create(
      mod_str + "/face_detect/haarcascades/frontalface_cuda.xml");
  }
  else {
    face_cascade.load(mod_str + "/face_detect/haarcascades/frontalface.xml");
  }
}

void FindFace(const v8::FunctionCallbackInfo<v8::Value>& args) {
  v8::Isolate* isolate = v8::Isolate::GetCurrent();
  v8::HandleScope scope(isolate);

  v8::String::Utf8Value col_string(args[0]->ToString());
  std::string col_path = std::string(*col_string);
  v8::Local<v8::Function> cb = v8::Local<v8::Function>::Cast(args[1]);
  const unsigned argc = 1;

  bool foundoneface = false;

  ////// CPU
  cv::Mat img_bw;
  cv::Mat img_c = imread(col_path, cv::IMREAD_COLOR);
  std::vector<cv::Rect> faces;

  /////// GPU
  cv::cuda::GpuMat img_c_gpu;
  cv::cuda::GpuMat img_bw_gpu;
  cv::cuda::GpuMat faces_gpu;

  if (img_c.empty()) {
    return;
  }
    if (img_c.channels() == 1) {
      v8::Local<v8::Value> argv[argc] = { v8::Integer::New(isolate, 0) };
      cb->Call(isolate->GetCurrentContext()->Global(), argc, argv);
    }
    else {
      cv::cvtColor(img_c, img_bw, cv::COLOR_BGR2GRAY);
    }
    cv::equalizeHist(img_bw, img_bw);
  if (bIsCuda) {
    img_c_gpu.upload(img_c);
    if (img_c.channels() == 3) {
      cv::cuda::cvtColor(img_c_gpu, img_bw_gpu, cv::COLOR_RGB2GRAY);
    }
    else if (img_c.channels() == 4) {
      cv::cuda::cvtColor(img_c_gpu, img_bw_gpu, cv::COLOR_RGBA2GRAY);
    }
    cascade_gpu->setFindLargestObject(false);
    cascade_gpu->setScaleFactor(1.2);
    cascade_gpu->setMinNeighbors(false);
    cascade_gpu->detectMultiScale(img_bw_gpu, faces_gpu);
    cascade_gpu->convert(faces_gpu, faces);
    if (faces.empty()) {
      std::cout << "No faces found" << std::endl;
    }
  }
  else {
    face_cascade.detectMultiScale(img_bw, faces, 1.1, 2,
                                  0|cv::CASCADE_SCALE_IMAGE, cv::Size(30, 30));
  }

  v8::Handle<v8::Array> array = v8::Array::New(isolate, 4);
  for (auto& face : faces) {
    cv::Point pt_top_left(face.x, face.y);
    int bottom_r_x = face.x + face.width;
    int bottom_r_y = face.y + face.height;
    if (bottom_r_x >= img_bw.size().width ||
        bottom_r_y >= img_bw.size().height) {
      continue;
    }
    cv::Point pt_bottom_right(face.x + face.width, face.y + face.height);
    if (face.width <= 0 || face.height <= 0) {
      continue;
    }
    cv::rectangle(img_c, pt_top_left, pt_bottom_right, cv::Scalar(0, 0, 255));
    // std::vector<cv::Rect> eyes;

    cv::Mat face_roi_bw = img_bw(face);
    // cv::Mat face_roi_c = img_c(face);

    if (!foundoneface) {
      array->Set(0, v8::Integer::New(isolate, pt_top_left.x));
      array->Set(1, v8::Integer::New(isolate, pt_top_left.y));
      array->Set(2, v8::Integer::New(isolate, pt_bottom_right.x));
      array->Set(3, v8::Integer::New(isolate, pt_bottom_right.y));
      // eyes_cascade.detectMultiScale(face_roi_bw, eyes, 1.1, 2,
      //                               0 |cv::CASCADE_SCALE_IMAGE,
      //                               cv::Size(30, 30));
      // if (eyes.size() > 0) {
      //   int i = 4;
      //   array->Set(i, v8::Integer::New(isolate, eyes.size()));
      //   for (auto& eye : eyes) {
      //       int eye_x = eye.x + eye.width/2;
      //       int eye_y = eye.y + eye.height/2;
      //       int radius = cvRound((eye.width + eye.height)*0.25);
      //       array->Set(++i, v8::Integer::New(isolate, eye_x));
      //       array->Set(++i, v8::Integer::New(isolate, eye_y));
      //       array->Set(++i, v8::Integer::New(isolate, radius));
      //       printf("eye.x: %d\teye.y: %d\tradius: %d\n", eye_x, eye_y,
      //               radius);
      //     }
      // }
      foundoneface = true;
    }
  }
  if (foundoneface) {
    v8::Local<v8::Value> argv[argc] = { array };
    cb->Call(isolate->GetCurrentContext()->Global(), argc, argv);
  }
}

void Init(v8::Handle<v8::Object> exports, v8::Handle<v8::Object> module) {
  NODE_SET_METHOD(exports, "init", LoadCascades);
  NODE_SET_METHOD(exports, "findface", FindFace);
}

NODE_MODULE(face_crawl, Init)