<?php
  $conn = @mysqli_connect("localhost","root","mwanje256","StringTaskDB");

  if($conn->connect_error){
    echo "Error: " . $conn->connect_error;
  }

  $file = '/var/www/html/Ready_list.txt';
  $handle = fopen($file, 'a+');
  if ($handle){
    while (($line = fgets($handle)) !== false){
      $cols = explode(' ', $line);

      $ID = (int)$cols[0];//The JobID
      $Result = $cols[1];//The processed result

      if(!mysqli_query($conn,"INSERT INTO Ready_list(JobID,Result) VALUES('$ID','$Result')")){
        echo "ERROR:Failed to insert into database table\n";
      }
    }
    fclose($handle);
  }

  file_put_contents('/var/www/html/Ready_list.txt','');//Clearing the Ready_list file
  fclose( $file );
  mysqli_close($conn);
?>
