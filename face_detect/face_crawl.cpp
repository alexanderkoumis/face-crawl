#include <node.h>
#include <opencv2/core/core.hpp> // cvRound
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp> // imread
#include <opencv2/imgproc/imgproc.hpp> // cvtColor

using namespace v8;

const static std::vector<int> imwriteargs = {cv::IMWRITE_JPEG_QUALITY, 95};
static cv::CascadeClassifier face_cascade;
static cv::CascadeClassifier eyes_cascade;



void LoadCascades(const FunctionCallbackInfo<Value>& args) {
  v8::String::Utf8Value mod_path(args[0]->ToString());
  std::string mod_str = std::string(*mod_path);
  face_cascade.load(mod_str + "/face_detect/haarcascades/frontalface.xml");
  // eyes_cascade.load(mod_str + "/face_detect/haarcascades/eye.xml");
}

void FindFace(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = Isolate::GetCurrent();
  HandleScope scope(isolate);

  v8::String::Utf8Value col_string(args[0]->ToString());
  std::string col_path = std::string(*col_string);
  v8::Local<v8::Function> cb = v8::Local<v8::Function>::Cast(args[1]);
  const unsigned argc = 1;

  cv::Mat img_bw;
  cv::Mat img_c = imread(col_path, cv::IMREAD_COLOR);
  std::vector<cv::Rect> faces;

  if (img_c.empty()) {
    return;
  }

  if (img_c.channels() == 1) {
    // v8::Local<v8::Value> val[arg] = v8::Integer::New(isolate, 0);
    v8::Local<v8::Value> argv[argc] = { v8::Integer::New(isolate, 0) };
    cb->Call(isolate->GetCurrentContext()->Global(), argc, argv);

    // img_bw = img_c;
  }
  else {
    cv::cvtColor(img_c, img_bw, cv::COLOR_BGR2GRAY);
  }
  cv::equalizeHist(img_bw, img_bw);

  bool foundoneface = false;
  v8::Handle<v8::Array> array = v8::Array::New(isolate, 4);
  face_cascade.detectMultiScale(img_bw, faces, 1.1, 2, 0|cv::CASCADE_SCALE_IMAGE, cv::Size(30, 30));

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
      // eyes_cascade.detectMultiScale(face_roi_bw, eyes, 1.1, 2, 0 |cv::CASCADE_SCALE_IMAGE, cv::Size(30, 30));
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
      //       printf("eye.x: %d\teye.y: %d\tradius: %d\n", eye_x, eye_y, radius);
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

void Init(Handle<Object> exports, Handle<Object> module) {
  NODE_SET_METHOD(exports, "init", LoadCascades);
  NODE_SET_METHOD(exports, "findface", FindFace);
}

NODE_MODULE(face_crawl, Init)
