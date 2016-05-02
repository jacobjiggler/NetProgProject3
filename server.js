// Load the http module to create an http server.
var http = require('http');
var filter = require('stream-filter');
function capitalize(data){
	return data.toUpperCase();
}


var host;

var host = process.argv[2];

// Configure our HTTP server to respond with Hello World to all requests.
var server = http.createServer(function (request, response) {
	console.log('serve: ' + request.url);

	var options = {
		hostname: host,
		port: 80,
		path: request.url,
		method: 'GET'
	};

	
	var proxy = http.request(options, function (res) {
		res.pipe(response, {
			end: true
		});
	});

	request.pipe(proxy, {
		end: true
	});

	//response.writeHead(200, {"Content-Type": "text/plain"});
	//response.end("Hello World\n");
});

// Listen on port 8000, IP defaults to 127.0.0.1
server.listen(8000);

// Put a friendly message on the terminal
console.log("Server running at http://127.0.0.1:8000/");
