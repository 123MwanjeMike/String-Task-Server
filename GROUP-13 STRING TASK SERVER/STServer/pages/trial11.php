<?php
$conn = @mysqli_connect("localhost","root","","StringTaskDB");

  if($conn->connect_error){
    echo "Error: " . $conn->connect_error;
  }

  $sql = "DELETE FROM busy_calc WHERE 1;";
  $sql .= "DELETE FROM trial WHERE 1;";

  $open = fopen('./Ready_list.txt','r');
 
while (!feof($open)) 
{
  $getTextLine = fgets($open);
  $explodeLine = explode("  ",$getTextLine);
  
  list($cip,$cjob,$priority,$total) = $explodeLine;
  
  $qry = "insert into busy_calc (ClientIP,JobType,Priority,Total) values('".$cip."','".$cjob."','".$priority."','".$total."')";
  mysqli_query($conn,$qry);
}
 
fclose($open);

  mysqli_close($conn);


      ?>
