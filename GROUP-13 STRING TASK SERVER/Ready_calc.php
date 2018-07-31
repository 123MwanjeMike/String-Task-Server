<?php
   $conn = @mysqli_connect("localhost","root","mwanje256","StringTaskDB");

  if($conn->connect_error){
    echo "Error: " . $conn->connect_error;
  }

  $file = '/var/www/html/ClientServer/Ready_list.txt';
  $handle = fopen($file, 'r');
  if ($handle){
	mysqli_query($conn,"DROP TABLE Ready_Calc;");
	mysqli_query($conn,"CREATE TABLE Ready_Calc(ClientID varchar(25),Job varchar(100),Priority int(1))");
    while (($line = fgets($handle)) !== false){
      $cols = explode(' ', $line);

      $JobID = $cols[0];
      $job = $cols[1];
      $priority =(int)$cols[2];

      if(!mysqli_query($conn,"INSERT INTO Ready_Calc(ClientID,Job,Priority) VALUES('$JobID','$job','$priority')")){
        echo "ERROR:Failed to insert into database table\n";
      }
    }
    fclose($handle);
  }

  fclose( $file );
  mysqli_close($conn); 
?>
