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
		
		$query="truncate carte";
		$rs=$dbh->prepare($query);
		
		$rs->execute();

		header('Access-Control-Allow-Origin: *');
		header('Access-Control-Allow-Methods: GET');
		header('Content-type: application/json');
	header("HTTP/1.1 200 OK");	


		
 
?>