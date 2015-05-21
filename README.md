# FaceCrawl

FaceCrawl is a CVaaS (Computer Vision as a Service) utility for scraping pictures from arbitrary sites on the net.

Video demo:

<a href="http://www.youtube.com/watch?feature=player_embedded&v=sMge1GxvCYk
" target="_blank"><img src="http://img.youtube.com/vi/sMge1GxvCYk/0.jpg" 
alt="FaceCrawl" width="240" height="180" border="10" /></a>

## Prerequisites

  * [nodejs/iojs] (https://nodejs.org/download/)
  * [phantomjs] (http://phantomjs.org/)
  * [OpenCV 3] (http://opencv.org/downloads.html)

## Installing

_Note_: Windows should work but is not officially supported

Installing project modules:

```bash
git clone https://github.com/alexanderkoumis/face-crawl
cd face-crawl
npm install
cd public
bower install
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

The FaceCrawl front-end client should now be running on your localhost. Try it out by navigating to localhost:3000`with your web browser.
