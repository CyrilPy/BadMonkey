/// @file freelancer.js
/// @author justine sabbatier
/// @version 1.5
/// @date 17/10/2014

////////////////////// VARIABLES GLOBALES

/// @def x		abscisse
var x;
/// @def y		ordonnées
var y;
/// @def robx	abscisse du robot
var robx;
/// @def roby		ordonnées du robot
var roby;
/// @def tobo 		orientation du robot
var tobo;
/// @def canvas		canvas du site affichant les obstacles
var canvas;
/// @def context		context du canvas
var context;
/// @def TO_RADIANS		valeur du radian
var TO_RADIANS = Math.PI/180;
/// @def robot		image du robot
 var robot = new Image();
    robot.src = 'img/robotCanvas.png'; 
 
 
 
////////////////////// FONCTIONS


/// @brief fonction qui recuperer le canvas et le contexte et appelle les fonctions de communications
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


/// @fn function comServer(method)
/// @brief fonction qui envoie un get au serveur
/// @param method
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

   
/// @fn function JsonCoord( monJSON)
/// @brief fonction qui évalue le json et recupere dans des variables les coordonées
/// @param monJSON		
function JsonCoord( monJSON)
{
	/*interpretation du json*/
	monJSON = eval(monJSON);
	/* parcours de l'array des coordonnées des obstacles*/
	for (var k in monJSON[1])
	{
		x=(monJSON[1][k][0][0]);
		y=(monJSON[1][k][0][1]);
		context.moveTo(x, y);
		x=(monJSON[1][k][1][0]);
		y=(monJSON[1][k][1][1]);
		context.lineTo(x, y);
	}
	/* coordonnées robots*/
	robo=(monJSON[0][2]);
	robx=(monJSON[0][0]);
	roby=(monJSON[0][1]);
	drawRotatedImage(robot, robx, roby, robo);
	console.log(robo);
	/*dessins*/
	context.stroke();//On trace seulement les lignes.
	context.closePath();	
} 


/// @fn function drawRotatedImage(image, x, y, angle)
/// @brief fonction qui effectue la rotation de l'image du robot en fonction de ses coordonnées
/// @param image ; variable contenant l'image
/// @param x ; variable des abscises
/// @param y ; variables des ordonnées
/// @param angle ; variable de l'angle d'orientation
function drawRotatedImage(image, x, y, angle) { 
	// save the current co-ordinate system 
	// before we screw with it
	context.save(); 
	// move to the middle of where we want to draw our image
	context.translate(x, y);
	// rotate around that point, converting our 
	// angle from degrees to radians 
	context.rotate(angle * TO_RADIANS);
	// draw it up and to the left by half the width
	// and height of the image 
	context.drawImage(image, -(image.width/2), -(image.height/2)); 
	// and restore the co-ords to how they were when we began
	context.restore(); 
}


/// @fn function initialize()
/// @brief fonction qui initialise la taille du canvas en fonction de la fenetre
function initialize() 
{
	// Register an event listener to
	// call the resizeCanvas() function each time 
	// the window is resized.
	window.addEventListener('resize', resizeCanvas, false);
	// Draw canvas border for the first time.
	resizeCanvas();
}


/// @fn function redraw()
/// @brief fonction qui redessine les traits en couleur #2c3e50 et en épaisseur de trait 5
function redraw() {
	context.strokeStyle = '#2c3e50';
	context.lineWidth = '5';
		}

/// @fn function redraw()
/// @brief Draw canvas border for the first time.
function resizeCanvas() 
{
	canvas.width = window.innerWidth;
	canvas.height = window.innerHeight;
	redraw();
}



////////////////////// TELECHARGER IMAGE


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


////////////////////// ACTION RAFRAICHISSEMENT

$("#connect").click(function (e)
{
	e.preventDefault();
	context.clearRect(0, 0, context.canvas.width, context.canvas.height);
	context.beginPath();//On démarre un nouveau tracé
	comServer(JsonCoord);
});


////////////////////// EFFETS SITE

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