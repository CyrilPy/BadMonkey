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
		$firstIteration = true;
		$rs->execute();
		$row = $rs->fetchAll();
		
		$jsonObstacle = array(array(array(),array()));
		
		
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

		}

		$json=array($jsonRobot,$jsonObstacle,$jsonMapPoint);
		header('Access-Control-Allow-Origin: *');
		header('Access-Control-Allow-Methods: GET');
		header('Content-type: application/json');
	header("HTTP/1.1 200 OK");	
	echo (json_encode ($json));

		
 
?>