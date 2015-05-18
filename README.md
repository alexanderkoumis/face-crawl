FaceCrawl is a CVaaS (Computer Vision as a Service) utility for scraping pictures from arbitrary sites on the net.

## Prerequisites

  * [nodejs/iojs] (https://nodejs.org/download/)
  * [phantomjs] (http://phantomjs.org/)
  * [OpenCV 2.4.9] (http://opencv.org/downloads.html)

## Installing

_Note_: Windows should work but is not officially supported

Installing project-specific Node modules:

```bash
git clone https://github.com/alexanderkoumis/face-crawl
cd face-crawl
npm install
```

Building face-detection C++ Node addon:

```bash
# Execute in project root
cd face_detect
node-gyp rebuild
```

## Running

Starting the server:

```bash
# Execute in project root
node index.js
```

The FaceCrawl front-end client should now be running on your localhost. Try it out by navigating to `localhost:3000` with your web browser.
