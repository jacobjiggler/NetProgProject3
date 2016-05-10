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
	console.log('host: ' + host + ' serve: ' + request.url);

	var options = {
		hostname: host,
		port: 80,
		path: request.url,
		method: 'GET'
	};

	var str = '';

	var proxy = http.request(options, function (res) {
		res.on('data', function(data){
			//console.log(data.toString());
			//response.writeHead(200, {"Content-Type": "text/plain"});
			//response.end(data.toString());
			str+=capitalize(data.toString());
		});

		res.on('end', function(){
			//response.writeHead(200, {"Content-Type": "text/plain"});
			response.end(str);
		});

		/*
		res.pipe(response, {
			end: true
		});
		*/
		
	});
	
	request.pipe(proxy, {
		end: true
	});
	
});





// Listen on port 8000, IP defaults to 127.0.0.1
server.listen(8000);

// Put a friendly message on the terminal
console.log("Server running at http://127.0.0.1:8000/");
