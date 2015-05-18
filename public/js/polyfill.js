(function(){ // polyfill for requestAnimationFrame
  var lastTime = 0;
  var vendors = ['ms', 'moz', 'webkit', 'o'];
  for(var x = 0; x < vendors.length && !window.requestAnimationFrame; ++x) {
    window.requestAnimationFrame = window[vendors[x]+'RequestAnimationFrame'];
    window.cancelAnimationFrame = window[vendors[x]+'CancelAnimationFrame'] || window[vendors[x]+'CancelRequestAnimationFrame'];
  }
  
  if (!window.requestAnimationFrame){
    window.requestAnimationFrame = function(callback, element){
      var currTime = new Date().getTime();
      var timeToCall = Math.max(0, 16 - (currTime - lastTime));
      var id = window.setTimeout(function(){ callback(currTime + timeToCall); }, timeToCall);
    lastTime = currTime + timeToCall;
      return id;
    };
  }

  if (!window.cancelAnimationFrame){
    window.cancelAnimationFrame = function(id){
      clearTimeout(id);
    };
  }
}());

(function(){ // polyfill for performance.now
	if (typeof window.performance === 'undefined') {
		window.performance = {};
	}
	 
	if (!window.performance.now){
		var nowOffset = Date.now();
		 
		if (performance.timing && performance.timing.navigationStart){
			nowOffset = performance.timing.navigationStart
		}
    
		window.performance.now = function now(){
			return Date.now() - nowOffset;
		}
	}
})();