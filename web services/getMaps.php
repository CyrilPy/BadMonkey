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
 


		try
        {
			$dbh = new PDO($dsn, $username, $password);
        }
        catch (PDOException $e)
        {
            die( "Erreur ! : " . $e->getMessage() );
        }
		
		$query="select * from carte order by time desc";
		$rs=$dbh->prepare($query);
		$json =array();
		$firstIteration = false;
		$rs->execute();
		$row = $rs->fetchAll();
		
		$jsonObstacle = array(array(array(),array()));
		echo"<pre>";
		print_r($jsonObstacle);
		echo("</pre>");
		
		foreach ($row as $tmp )
		{
			$xRobot = $tmp['xRobot'];
			$yRobot = $tmp['yRobot']; 
			$xMap   = $tmp['xMap'];
			$yMap   = $tmp['yMap'];
			$alpha  = $tmp['alphaRobot'];
			$date   = $tmp['time'];
			if($firstIteration)
			{
				$jsonRobot= array($xRobot,$yRobot,$alpha);
				$firstIteration= false;
			}
			$jsonMapPoint[] = array($xMap,$yMap);
			print_r("json map est remplis avec et donne :<br> ");
			print_r($jsonMapPoint );
			echo("la valeur en x est $xMap et y vaut $yMap");
		}
		echo"<pre>";
		print_r($jsonMapPoint);
		echo("</pre>");
		$json=array($json
		header('Access-Control-Allow-Origin: *');
		header('Access-Control-Allow-Methods: GET');
	header("HTTP/1.1 200 OK");	
	echo "done";

		
 
?>