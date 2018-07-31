<?php
  
  $conn = @mysqli_connect("localhost","root","mwanje256","StringTaskDB");

  if($conn->connect_error){
    echo "Error: " . $conn->connect_error;
  }

  $file = 'Busy_list.txt';
  $handle = fopen($file, 'a+');
  if ($handle){
    while (($line = fgets($handle)) !== false){
      $cols = explode('	', $line);

      $cip = $cols[0];
      $jobtype = $cols[1];
      $priority = (int)$cols[2];
      $total = (int)$cols[3];

      
      mysqli_query($conn,"DELETE *FROM Busy_calc");
      if(!mysqli_query($conn,"INSERT INTO Busy_calc(ClientIP,JobType,Priority,Total)         
      VALUES('$cip','$jobtype','$priority','$total')")){
        echo "ERROR:Failed to insert into database table\n";
      }
      $result = mysqli_query($conn,"CALL failure()");
      $posts = mysqli_fetch_all($result,MYSQLI_ASSOC);
     

      ?>




<?php
    }
    fclose($handle);
  }

  file_put_contents('/var/www/html/Busy_list.txt','');
  fclose( $file );
  
  mysqli_free_result($result);
  mysqli_close($conn);
?>
