<?php
   $conn = @mysqli_connect("localhost","root","mwanje256","StringTaskDB");

  if($conn->connect_error){
    echo "Error: " . $conn->connect_error;
  }

  $file = '/var/www/html/ClientServer/Busy_list.txt';
  $handle = fopen($file, 'r');
  if ($handle){
	mysqli_query($conn,"DROP TABLE Busy_list;");
	mysqli_query($conn,"CREATE TABLE Busy_list(ID int(3) PRIMARY KEY AUTO_INCREMENT,ClientID varchar(25),Job varchar(100),Priority int(1),total int(3))");
    while (($line = fgets($handle)) !== false){
      $cols = explode('	', $line);

      $ip = $cols[0];//The JobID
      $job = $cols[1];
      $priority =(int) $cols[2];
      $total = (int)$cols[3];
      //The processed result

      if(!mysqli_query($conn,"INSERT INTO Busy_list(ClientID,Job,Priority,total) VALUES('$ip','$job','$priority','$total')")){
        echo "ERROR:Failed to insert into database table\n";
      }
    }
    fclose($handle);
  }

  fclose( $file );
  mysqli_close($conn); 
?>
