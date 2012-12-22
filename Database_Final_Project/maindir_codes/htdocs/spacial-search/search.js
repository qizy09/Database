var mymarker;
var map;
var markersArray = [];
var infoWind = []
var MAX_RESULTS_NUM = 5;
function initializeMap() {
    var latlng = new google.maps.LatLng(1.6, 103.7);//39.90867245598663, 116.39749873201185);
    var myOptions = {
        zoom: 12,
        center: latlng,
        mapTypeId: google.maps.MapTypeId.ROADMAP
    };
    map = new google.maps.Map (document.getElementById("map_canvas"), myOptions);
    mymarker = new google.maps.Marker({
        position: latlng, 
        map: map,
        title:[latlng.lat(), latlng.lng()].join(", "),
        icon: "http://maps.google.com/mapfiles/marker_purple.png",
/*
http://mabp.kiev.ua/2010/01/12/google-map-markers/
http://hi.baidu.com/bondbond/item/9e5ce72c13b62186ae48f523
 - http://maps.google.com/mapfiles/marker.png
 - http://maps.google.com/mapfiles/marker_black.png
 - http://maps.google.com/mapfiles/marker_grey.png
 - http://maps.google.com/mapfiles/marker_orange.png
 - http://maps.google.com/mapfiles/marker_white.png
 - http://maps.google.com/mapfiles/marker_yellow.png
 - http://maps.google.com/mapfiles/marker_purple.png
 - http://maps.google.com/mapfiles/marker_green.png
 - http://maps.google.com/mapfiles/shadow50.png
 - http://maps.gstatic.com/intl/en_ALL/mapfiles/drag_cross_67_16.png
*/
        draggable: true
    }); 
    map.setCenter(mymarker.getPosition());
    google.maps.event.addListener(mymarker, 'click', function() 
	{
        map.setCenter(mymarker.getPosition());
    });
 //   google.maps.event.addListener(mymarker, 'dragstart', function() {
//        updateMarkerAddress('Dragging...');
   // });
  
 //   google.maps.event.addListener(mymarker, 'drag', function() {
  //  });
  
    google.maps.event.addListener(mymarker, 'dragend', function() {
//        updateMarkerStatus('Drag ended');
//        geocodePosition(mymarker.getPosition());
        var pos = mymarker.getPosition();
        mymarker.setTitle([pos.lat(), pos.lng()].join(", "));
        searchKeywords();
    });
    
    for (var i = 0; i < MAX_RESULTS_NUM; ++i) {
        markersArray[i] = new google.maps.Marker({icon: "http://maps.google.com/mapfiles/marker.png"});
    }
}

function searchKeywords() {
    var query = document.getElementById("querytext").value;
//	document.getElementById("showquery").innerHTML = query;
    if(query.length == 0) {
        return;
	}
    doSearch(query);
}

var xmlHttpRequest;
//XmlHttpRequest对象  
function createXmlHttpRequest(){  
    if(window.ActiveXObject){ //如果是IE浏览器  
        return new ActiveXObject("Microsoft.XMLHTTP");  
    }
    else if(window.XMLHttpRequest){ //非IE浏览器  
        return new XMLHttpRequest();  
    }  else {
        return new XMLHttpRequest(); 
    }
} 

function doSearch(query) {
    var url = "http://localhost:8080/fcgi-bin/SpacialSearch4.fcgi?" + "keyword=" + query + "&lat=" + mymarker.getPosition().lat()+ "&lng=" + mymarker.getPosition().lng();//"http://59.66.133.37/fcgi-bin/SpacialSearch.fcgi";  //initializeMap' + location.host +"
	alert(url);
    url = encodeURI(url);
    //1.创建XMLHttpRequest组建  
    xmlHttpRequest = createXmlHttpRequest();  
    //2.设置回调函数  
    xmlHttpRequest.onreadystatechange = handleIpResponse;  
    //3.初始化XMLHttpRequest组建  
    xmlHttpRequest.open("GET", url, true);  
    //4.发送请求  
    xmlHttpRequest.send(null);   
}

function gen_result(content_item) {
    var inner = "<p>" + "<span style=\"color:blue;\">" + content_item.name + "</span><br />";
    inner += "<span style=\"color:grey; font-size:9pt\">" + content_item.addr + "</span> <br />";
    inner += "<span style=\"color:grey; font-size:9pt\">" + content_item.url + "</span> <br />";
    var html = "<li style=\"list-style:none;text-align:left;\">" + inner + "</li>";
    return html;
}

function handleIpResponse(){  
    if(xmlHttpRequest.readyState == 4) {  
        var result = xmlHttpRequest.responseText;  
		alert(result);
        var response = eval('(' + result + ')');
        var run_time = response.time;
        var content = response.content;
        var bounds = new google.maps.LatLngBounds();
        bounds.extend(mymarker.getPosition());
        document.getElementById("results").innerHTML = "";
		
		//alert(content.length);
        for (var i = 0; i < content.length; i++) {
            var result_item = content[i];
            var marker_item = markersArray[i];
			var newPos = new google.maps.LatLng(result_item.lat, result_item.lng);
            marker_item.setPosition(newPos);
            marker_item.setMap(map);
            marker_item.setTitle(result_item.name);
			marker_item.setClickable(true);
			//alert(result_item.url);
			marker_item.info = new google.maps.InfoWindow({
				content: '<div id = "yeah"  style="background: pink;"> <b> <font size="3" color="blue"> ' + result_item.name + ' </font></b> <br/>  <font size="2" color="black"> <b> Addr: </b>' + result_item.addr + ', ' + result_item.pcode + '<br/> <a href="' + result_item.url + '"> <style>img{border:0;}</style><img src="http://gothere.sg/static/img/2/icon/map/a.png" height = 8% width = 8%/> Hang Around</a></font></div>'
			});
			
			marker_item.info.setOptions({maxWidth: 110});
			marker_item.info.setOptions({zIndex: i});
			google.maps.event.addListener(marker_item, 'click', function() {
				marker_item.info.open(map, marker_item);
			});
            bounds.extend(marker_item.getPosition());
            document.getElementById("results").innerHTML = document.getElementById("results").innerHTML + gen_result(content[i]);
        }
        for (var i = content.length; i < MAX_RESULTS_NUM; ++i) {
            markersArray[i].setMap(null);
        }
        if (content.length > 0)
            map.fitBounds(bounds);   
        document.getElementById("comment").innerHTML = "<span style=\"color:red;\">" + response.count + "</span>" + " results in " + "<span style=\"color:red;\">" + response.time + "</span>" + " seconds" + "</span>";
    }  
} 

