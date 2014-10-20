<?php
    /**
	@file connect.php
    @brief Connection parameters of the database
    @details This file content the connections parameters. It must be included in all file who need to connect with the database.
    @author Mathias Da Costa
    @code require_once("./connect.php"); @endcode
	 @details Date of modification   :   20/10/2014
*   @date       20/10/2014
*/
 
$hostname = "mysql.imerir.com";
$database = "test";
$username = "eevee";
$password = "evoli";
$dsn = "mysql:host=$hostname;dbname=$database";
 
$xRobot = $_GET['xr'];
$yRobot = $_GET['yr'];
$xMap   = $_GET['xm'];
$yMap   = $_GET['ym'];
$alpha  = $_GET['a'];
$date = time();
echo $date;

		try
        {
			$dbh = new PDO($dsn, $username, $password);
        }
        catch (PDOException $e)
        {
            die( "Erreur ! : " . $e->getMessage() );
        }
		
		$query="insert into carte (xMap,yMap,xRobot,yRobot,alphaRobot,time) 
				values (:xMap,:yMap,:xRobot,:yRobot,:alpha,:time)";
		$rs=$dbh->prepare($query);
		$rs->bindParam(':xRobot',$xRobot);
		$rs->bindParam(':yRobot',$yRobot);
		$rs->bindParam(':xMap',$xMap);
		$rs->bindParam(':yMap',$yMap);
		$rs->bindParam(':alpha',$alpha);
		$rs->bindParam(':time',$date);
		
		$rs->execute();
		header('Access-Control-Allow-Origin: *');
		header('Access-Control-Allow-Methods: GET');
	header("HTTP/1.1 200 OK");	
	echo "done";

		
 
?>