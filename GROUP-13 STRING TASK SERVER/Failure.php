<?php

   $conn = @mysqli_connect("localhost","root","mwanje256","StringTaskDB");

  if($conn->connect_error){
    echo "Error: " . $conn->connect_error;
  }

	mysqli_query($conn,"DROP TABLE Busy_calc1;");
	mysqli_query($conn,"DELETE FROM trial WHERE 1;");
	mysqli_query($conn,"CREATE TABLE Busy_calc1(ClientID varchar(30) , JobType varchar(70),Priority int(1),Total int(3));");


  $file = '/var/www/html/ClientServer/Busy_list.txt';
  $handle = fopen($file, 'r');
if(!$handle){
echo 'failed to open';
}

  if ($handle){
	
    while (($line = fgets($handle)) !== false){
      $cols = explode('	', $line);

      $ip = $cols[0];//The JobID
      $job = $cols[1];
      $priority =(int) $cols[2];
      $total = (int)$cols[3];
      //The processed result
	$result = mysqli_query($conn,"INSERT INTO Busy_calc1(ClientID,JobType,Priority,Total) VALUES('$ip','$job','$priority','$total')");

      if(!$result){
        echo "ERROR:Failed to insert into database table\n";
      }

    }
    mysqli_query($conn,"CALL Failure_proc();");

   
    fclose($handle);
  }

  fclose( $file );
  mysqli_close($conn);
?>
