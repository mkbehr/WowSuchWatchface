var satoshiPerDoge = -1;

function updateSatoshiPerDoge(callback){
	var req = new XMLHttpRequest();
	req.open('GET', 'https://vircurex.com/api/get_last_trade.json?base=DOGE&alt=BTC', true);
	req.onload = function(e){
		if (req.readyState == 4 && req.status == 200) {
			var response = JSON.parse(req.responseText);
			var btcPerDoge = -1;
			try {
				btcPerDoge = parseFloat(response.value);	
			} catch (err){
			}
			if (isNaN(btcPerDoge) || btcPerDoge < 0){
				satoshiPerDoge = -1;
			} else {
				satoshiPerDoge = Math.round(btcPerDoge * 100000000);
				console.log("Fetched btc/doge data: " + satoshiPerDoge + " satoshi per doge");
			}
		} else {
			satoshiPerDoge = -1;
		}
		callback(satoshiPerDoge);
	}
	req.send();
}

function spdCallback(spd){
	var transactionId = Pebble.sendAppMessage(
	{"0" : spd},
	 function(e){
		 console.log("Successfully delivered message with transactionId="
			 + e.data.transactionId);
	 },
	 function(e){
		 console.log("Unable to deliver message with transactionId="
			 + e.data.transactionId
			 + " Error is: " + e.error.message);
	 });
}

function mainLoop(){
	updateSatoshiPerDoge(spdCallback);
}

Pebble.addEventListener(
	"ready",
	 function(e){
		 mainLoop();
		 setInterval(mainLoop, 60 * 1000);
	 });