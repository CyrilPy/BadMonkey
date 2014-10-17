
 
//var monJSON = eval("[[[30,50],[120,70]],[[200,250],[200,290]],[[110,50],[150,120]]]");
var x;
var y;
var canvas;
var context;


 
 /// @def data Json 
var data;
 
/// @fn function comServer(method, method2)
/// @brief Courte description.ajax fonction for connecting supervisor and server
/// @param method; here function jsonTable(data)		
/// @param method; here function mapCoord(data)
   

function JsonCoord( monJSON)
{
	monJSON = eval(monJSON);
	for (var k in monJSON)
	{
		x=(monJSON[k][0][0]);
		y=(monJSON[k][0][1]);
		context.moveTo(x, y);
		x=(monJSON[k][1][0]);
		y=(monJSON[k][1][1]);
		context.lineTo(x, y);
		console.log(x);
	}
	context.stroke();//On trace seulement les lignes.
	context.closePath();
} 

function comServer(method)
{
	  $.ajax({
       url :$('#adress').val(), // La ressource ciblée
       type : 'GET', // Le type de la requête HTTP.
	   statusCode : {
		   404 : function (){alert( "Server offline")}
	   }
    }).done(function( msg ){method(msg);});
   } 

window.onload = function()
{	
     canvas = document.getElementById('myCanvas');
        if(!canvas)
        {
            alert("Impossible de récupérer le canvas");
            return;
        }

     context = canvas.getContext('2d');
        if(!context)
        {
            alert("Impossible de récupérer le context du canvas");
            return;
        }
		
		
		initialize(canvas, context);
		context.beginPath();//On démarre un nouveau tracé
		comServer(JsonCoord);


}

/************TELECHARGER IMAGE***********/


/**
 * This is the function that will take care of image extracting and
 * setting proper filename for the download.
 * IMPORTANT: Call it from within a onclick event.
*/
function downloadCanvas(link, canvasId, filename) {
    link.href = document.getElementById(canvasId).toDataURL();
    link.download = filename;
}

/** 
 * The event handler for the link's onclick event. We give THIS as a
 * parameter (=the link element), ID of the canvas and a filename.
*/
document.getElementById('download').addEventListener('click', function() {
    downloadCanvas(this, 'myCanvas', 'carte.png');
}, false)

function initialize() {
// Register an event listener to
// call the resizeCanvas() function each time 
// the window is resized.
window.addEventListener('resize', resizeCanvas, false);

// Draw canvas border for the first time.
resizeCanvas();
}

function redraw() {
		context.strokeStyle = '#2c3e50';
		context.lineWidth = '5';
		context.strokeRect(0, 0, window.innerWidth, window.innerHeight);
            }

function resizeCanvas() 
{
	canvas.width = window.innerWidth;
	canvas.height = window.innerHeight;
	redraw();
}

$("#connect").click(function (e)
{
	e.preventDefault();
	context.clearRect(0, 0, context.canvas.width, context.canvas.height);
	context.beginPath();//On démarre un nouveau tracé
	comServer(JsonCoord);
});


/************EFFETS SITE***********/

// jQuery for page scrolling feature - requires jQuery Easing plugin
$(function() {
    $('.page-scroll a').bind('click', function(event) {
        var $anchor = $(this);
        $('html, body').stop().animate({
            scrollTop: $($anchor.attr('href')).offset().top
        }, 1500, 'easeInOutExpo');
        event.preventDefault();
    });
});

// Floating label headings for the contact form
$(function() {
    $("body").on("input propertychange", ".floating-label-form-group", function(e) {
        $(this).toggleClass("floating-label-form-group-with-value", !! $(e.target).val());
    }).on("focus", ".floating-label-form-group", function() {
        $(this).addClass("floating-label-form-group-with-focus");
    }).on("blur", ".floating-label-form-group", function() {
        $(this).removeClass("floating-label-form-group-with-focus");
    });
});

// Highlight the top nav as scrolling occurs
$('body').scrollspy({
    target: '.navbar-fixed-top'
})

// Closes the Responsive Menu on Menu Item Click
$('.navbar-collapse ul li a').click(function() {
    $('.navbar-toggle:visible').click();
});