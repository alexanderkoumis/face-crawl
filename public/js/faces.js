var socket = io();

onload = function() {
    setTimeout(init, 0);
}

DisplayMode = {
    WALL: 1,
    BURST: 2
}

init = function () {
    canvas = document.getElementById('init_canvas');
    context = canvas.getContext('2d');

    mouse = {
        x: canvas.width/2,
        y: canvas.height/2,
        out: false
    }

    canvas.onmouseout = function(){
        mouse.out = true;
    }

    canvas.onmousemove = function(e){
        var rect = canvas.getBoundingClientRect();
        mouse = {
          x: e.clientX - rect.left,
          y: e.clientY - rect.top,
          out: false
        }
    }

    onresize = function() {
        canvas.width = canvas.clientWidth;
        canvas.height = canvas.clientHeight;
    }
    window.onresize();

    socket.on('face', function(facesObj) {
        facesObj.rects.forEach(function (rect) {
            newFace(faces, facesObj.url, rect);
        });
        requestAnimationFrame(loop);
    });

    faces = [];

    //// Burst settings
    gravityStrength = 14;
    spawnTimer = 0;
    spawnInterval = 50;
    type = 0
    time = performance.now();
    maxFaceLimit = 50;

    //// Wall settings
    wall_x = 0;
    wall_y = 0;
    box_sz = 100;
    last_faces_len = 0;

    dispMode = DisplayMode.WALL;
    changedBurstToWall = false;

    loop();
}

loop = function() {
    if (dispMode == DisplayMode.BURST) {
        drawBurst();
        calcBurst();
    }
    else if (dispMode == DisplayMode.WALL) {
        drawWall();
    }
    requestAnimationFrame(loop);
}

newFace = function(faces, url, rect) {
    type = type ? 0 : 1; // 50% true / 50% false
    var img = new Image();
    img.onload = function() {
        faces.push({
            rect: rect,
            w: rect[2] - rect[0],
            h: rect[3] - rect[1],
            pic: img,
            x: canvas.width/2,
            y: canvas.height/2,
            xv: type ? 12 * Math.random() - 9 : 24 * Math.random() - 12,
            yv: type ? 18 * Math.random() - 9 : 24 * Math.random() - 12
        });
    }
    img.src = url;
}

drawBurst = function() {
    context.clearRect(0, 0, canvas.width, canvas.height);
    for (var i = 0; i < faces.length; ++i) {
        face = faces[i];
        // drawImage(img, x to start clipping, y to start clipping,
        //           width of clipped image, height of clipped image
        //           x to place image, y to place image,
        //           width to stretch/reduce to, height to stretch/reduce to)
        context.drawImage(face.pic, face.rect[0], face.rect[1], face.w, face.h,
                          face.x, face.y, 100, 100);
    }
}

calcBurst = function() {
    var dt = -time+(time=performance.now())
    spawnTimer += (dt<100) ? dt : 100;

    var faceOverflow = faces.length - maxFaceLimit;
    if (faceOverflow > 0) {
        faces.splice(0, faceOverflow);
    }
    for (var i=0; i < faces.length; ++i){
        var f = faces[i];
        if (!mouse.out) {
            x = mouse.x-f.x;
            y = mouse.y-f.y;
            a = x * x + y * y;
            a = a > 100 ? (gravityStrength/a) : (gravityStrength/100);
            f.xv = (f.xv + a*x) * 0.99;
            f.yv = (f.yv + a*y) * 0.99;
        }
        f.x += f.xv;
        f.y += f.yv;
    }
}

drawWall = function() {
    // context.clearRect(0, 0, canvas.width, canvas.height);
    startIdx = (changedBurstToWall) ? 0 : last_faces_len;
    changedBurstToWall = false;
    if (faces.length != startIdx) {
        for (var i = startIdx; i < faces.length; ++i) {
            face = faces[i];
            // tl.x, tl.y, tl.x, tl.y
            context.drawImage(face.pic, face.rect[0], face.rect[1], face.w, face.h,
                              wall_x, wall_y, box_sz, box_sz);
            wall_x += box_sz;
            if (wall_x >= canvas.width) {
                wall_y += box_sz;
                wall_x = 0;
            }
            if (wall_y >= canvas.height) {
                wall_x = 0;
                wall_y = 0;
            }
        }
    }
    last_faces_len = faces.length;
}

String.prototype.startsWith = function(sub_str) {
    return(this.indexOf(sub_str) == 0);
}

function scrapeSite(form) {
    site_str = form.site.value;
    console.log('you entered: ' + site_str);
    socket.emit('scrape_site', site_str);
}

function changeMode(newDispMode) {
    changedBurstToWall = (dispMode == DisplayMode.BURST &&
                          newDispMode == DisplayMode.WALL) ? true : false;
    dispMode = newDispMode;
    console.log('changed display mode to ' + dispMode);
}
