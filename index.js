var fs = require('fs');
var phantom = require('phantom');
var request = require('request');

var express = require('express');
var app = express();
var server = require('http').createServer(app);
var io = require('socket.io')(server);

var face_detect = require('./face_detect/build/Release/face_crawl');
face_detect.init(__dirname);

server.listen(3000, function() {
    console.log("Started server on port 3000");
})

app.use(express.static(__dirname + '/public'));

var collected_prefix = __dirname + "/images/collected/img";
var generated_prefix = __dirname + "/images/generated/img";

var img_cnt = 0;

io.on('connection', function(socket) {
    console.log('User connected');
    socket.on('scrape_site', function(site) {
        scrapeIt(socket, site);
    });
});

scrapeIt = function(socket, site) {
    phantom.create(function (ph) {
        ph.createPage(function (page) {
            page.set('onResourceReceived', function (response) {
                if (response.contentType != null) {
                    var url = response.url;
                    var content = response.contentType.toLowerCase();
                    var header = content.substr(0, "image/".length);
                    var ext =  content.substr("image/".length, content.length);
                    if ((ext != 'gif') && (ext != 'svg+xml') && (header == "image/")) {
                        request.get({url: response.url, encoding: 'binary'},
                                function (err, response, body)  {
                            if (!err && response.headers['content-length'] > 10000) {
                                var col_name = collected_prefix + img_cnt++ + "." + ext;
                                console.log("Wrote file: " + col_name);
                                fs.writeFile(col_name, body, 'binary', function() {
                                    face_detect.findface(col_name, function(rect) {
                                        var face = {'url': url, 'rect': rect};
                                        socket.emit('face', face);
                                    });
                                });
                            }
                        });
                    }
                }
            });
            console.log("Now crawling " + site);
            page.open(site, function(status) {
                ph.exit();
            });
        });
    });
}
