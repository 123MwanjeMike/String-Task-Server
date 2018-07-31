<?php
session_start();

if(!isset($_SESSION['Username'])){
header('location:login.php?error=***SESSION TIME OUT, PLEASE LOGIN AGAIN!***');
}
?>

<!DOCTYPE html>
<html lang="en">
  <head>
    <meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
    <!-- Meta, title, CSS, favicons, etc. -->
    <meta charset="utf-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <!-- REFRESH PAGE -->
    <meta http-equiv="refresh" content="30" />
	<link rel="icon" href="images/favicon.ico" type="image/ico" />

    <title>Group-13 String Task Server</title>

    <!-- Bootstrap -->
    <link href="../vendors/bootstrap/dist/css/bootstrap.min.css" rel="stylesheet">
    <!-- Font Awesome -->
    <link href="../vendors/font-awesome/css/font-awesome.min.css" rel="stylesheet">
    <!-- NProgress -->
    <link href="../vendors/nprogress/nprogress.css" rel="stylesheet">
    <!-- iCheck -->
    <link href="../vendors/iCheck/skins/flat/green.css" rel="stylesheet">
  
    <!-- bootstrap-progressbar -->
    <link href="../vendors/bootstrap-progressbar/css/bootstrap-progressbar-3.3.4.min.css" rel="stylesheet">
    <!-- JQVMap -->
    <link href="../vendors/jqvmap/dist/jqvmap.min.css" rel="stylesheet"/>
    <!-- bootstrap-daterangepicker -->
    <link href="../vendors/bootstrap-daterangepicker/daterangepicker.css" rel="stylesheet">

    <!-- Custom Theme Style -->
    <link href="../build/css/custom.min.css" rel="stylesheet">
  </head>

  <body class="nav-md">
    <div class="container body">
      <div class="main_container">
        <div class="col-md-3 left_col">
          <div class="left_col scroll-view">
            <div class="navbar nav_title" style="border: 0;">
              <a href="index.html" class="site_title"><i class="fa fa-paw"></i> <span>Group 13</span></a>
            </div>

            <div class="clearfix"></div>

            <!-- menu profile quick info -->
            <div class="profile clearfix">
              <div class="profile_pic">
                <img src="images/img.jpg" alt="..." class=" img-circle profile_img">
              </div>
              <div class="profile_info">
                <span>Welcome,</span>
                <h2><?php echo $_SESSION['Username'];?></h2>
              </div>
            </div>
            <!-- /menu profile quick info -->

            <br />

            <!-- sidebar menu -->
            <div id="sidebar-menu" class="main_menu_side hidden-print main_menu">
              <div class="menu_section">
                <h3>General</h3>
                <ul class="nav side-menu">
                  <li><a><i class="fa fa-home"></i> Home <span class="fa fa-chevron-down"></span></a>
                    <ul class="nav child_menu">
                      <li><a href="index.php">Dashboard</a></li>
                    </ul>
                  </li>
                  </li>
                  <li><a><i class="fa fa-table"></i> Jobs <span class="fa fa-chevron-down"></span></a>
                    <ul class="nav child_menu">
                      <li><a href="ready.php">Ready List</a></li>
			<li><a href="readyCalc.php">Ready list with priorites</a></li>
                      
			<li><a href="busyCalc.php">Busy list with priorites</a></li>
                      <li><a href="blacklist.php">Blacklisted Jobs</a></li>
                      </ul>
                  </li>
                  <li><a><i class="fa fa-bar-chart-o"></i> Task Information <span class="fa fa-chevron-down"></span></a>
                    <ul class="nav child_menu">
                      <li><a href="logs.php">Process Logs</a></li>
                      <li><a href="1success_rate.php">Success Rate</a></li>
                      <li><a href="failure_rate.php">Failure Rate</a></li>
                    </ul>
                  </li>

                  </li>
                </ul>
              </div>

              </div>

            </div>
            <!-- /sidebar menu -->

           
            <!-- /menu footer buttons -->
          </div>
        </div>

        <!-- top navigation -->
        <div class="top_nav">
          <div class="nav_menu">
            <nav>
              <div class="nav toggle">
                <a id="menu_toggle"><i class="fa fa-bars"></i></a>
              </div>

              <!-- Date presentation -->
              <h4 class="nav navbar-nav navbar-right">
                 <?php 
                date_default_timezone_set("Africa/Nairobi");
                 echo date("h:i a");?>
                <small><?php echo date("l-d,m-Y");?></small> 
              </h4>

              <!-- Date presentation -->


              <ul class="nav navbar-nav navbar-right">
                <li class="">
                  <a href="javascript:;" class="user-profile dropdown-toggle" data-toggle="dropdown" aria-expanded="false">
                    <img src="images/img.jpg" alt=""><?php echo $_SESSION['Username'];?>
                    <span class=" fa fa-angle-down"></span>
                  </a>
                  <ul class="dropdown-menu dropdown-usermenu pull-right">
                    <li><a href="./php/logout.php"><i class="fa fa-sign-out pull-right"></i> Log Out</a></li>
                  </ul>
                </li>

                <li role="presentation" class="dropdown">
                  <a href="javascript:;" class="dropdown-toggle info-number" data-toggle="dropdown" aria-expanded="false">
                  </a>
                </li>
              </ul>
            </nav>
          </div>
        </div>
        <!-- /top navigation -->

        <!-- page content -->
        <div class="right_col" role="main">
          <!-- top tiles -->
          <div class="row tile_count">
            <div class="col-md-2 col-sm-4 col-xs-6 tile_stats_count">
              <span class="count_top"><i class="fa fa-user"></i> Total Clients</span>
              <div class="count">

<!--display total Clients -->
          <?php
Require "./php/config.php";
$result = mysqli_query($con, "SELECT COUNT(DISTINCT ClientIP, ClientPort,Submission_date,Submission_time) as TotalClients FROM Logs") or die(mysqli_error($con));

if ($result->num_rows > 0) {
    // output data
  while($row = $result->fetch_assoc()) {

        echo $row["TotalClients"];
                  
    }
  }
?>
</div>
            </div>
            <div class="col-md-2 col-sm-4 col-xs-6 tile_stats_count">
              <span class="count_top"><i class="fa fa-clock-o"></i> Average Process Time</span>
              <div class="count"><h4>
                <!-- AVERAGE TIME -->

                <?php
include "./php/config.php";
$result = mysqli_query($con, "SELECT AVG(Duration_sec) FROM Logs") or die(mysqli_error($con));

if ($result->num_rows > 0) {
    // output data
  while($row = $result->fetch_assoc()) {

        echo $row["AVG(Duration_sec)"];
                  
    }
  }
?>(s)
              </h4>
      </div>
            </div>

            <div class="col-md-3.5 col-sm-4 col-xs-6 tile_stats_count">
              <span class="count_top"><i class="fa fa-clock-o"></i> Average Processing rate</span>
              <div class="count"><h4>
                <!-- Average processing rate -->

                <?php
include "./php/config.php";
$result = mysqli_query($con, "SELECT AVG((JobID / Duration_sec)) FROM Logs") or die(mysqli_error($con));

if ($result->num_rows > 0) {
    // output data
  while($row = $result->fetch_assoc()) {

        echo $row["AVG((JobID / Duration_sec))"];
                  
    }
  }
?> Jobs/sec
              </h4>
      </div>
            </div>
            <div class="col-md-2 col-sm-4 col-xs-6 tile_stats_count">
              <span class="count_top"><i class="fa fa-user"></i> Total Jobs</span>
              <div class="count green">
        <!-- total jobs -->

          <?php
include "./php/config.php";
$result = mysqli_query($con, "SELECT COUNT(DISTINCT JobID) FROM Logs") or die(mysqli_error($con));

if ($result->num_rows > 0) {
    // output data
  while($row = $result->fetch_assoc()) {

        echo $row["COUNT(DISTINCT JobID)"];
                  
    }
  }
?>
  
            </div>
            </div>
            <div class="col-md-2 col-sm-4 col-xs-6 tile_stats_count">
              <span class="count_top"><i class="fa fa-user"></i> Blacklisted Jobs</span>
              <div class="count">
            <!-- Blacklisted jobs -->

                <?php
include "./php/config.php";
$result = mysqli_query($con, "SELECT COUNT(DISTINCT JobID) FROM Black_list") or die(mysqli_error($con));

if ($result->num_rows > 0) {
    // output data
  while($row = $result->fetch_assoc()) {

        echo $row["COUNT(DISTINCT JobID)"];
                  
    }
  }
?>
  
</div>
            </div>
            
            </div>
          <!-- /top tiles -->
          <hr>
          <h5>This is a summary of the server operations</h5>
          <div class="row">
            <div class="col-md-12 col-sm-12 col-xs-12">

<div class="row top_tiles">
              <div class="animated flipInY col-lg-3 col-md-3 col-sm-6 col-xs-12">
                <div class="tile-stats">
                  <div class="icon"><i class="fa fa-caret-square-o-right"></i></div>
                  <div class="count">
                    <!-- Busy jobs -->

                <?php
include "./php/config.php";
$result = mysqli_query($con, "SELECT COUNT(DISTINCT ID) FROM Busy_calc") or die(mysqli_error($con));

if ($result->num_rows > 0) {
    // output data
  while($row = $result->fetch_assoc()) {

        echo $row["COUNT(DISTINCT ID)"];
                  
    }
  }
?></div>
                  <h3>Busy Jobs</h3>
                </div>
              </div>
              
              <div class="animated flipInY col-lg-3 col-md-3 col-sm-6 col-xs-12">
                <div class="tile-stats">
                  <div class="icon"><i class="fa fa-sort-amount-desc"></i></div>
                  <div class="count">
                    <!-- Prioritised jobs -->

                <?php
include "./php/config.php";
$result = mysqli_query($con, "SELECT COUNT(DISTINCT Priority) FROM Busy_calc") or die(mysqli_error($con));

if ($result->num_rows > 0) {
    // output data
  while($row = $result->fetch_assoc()) {

        echo $row["COUNT(DISTINCT Priority)"];
                  
    }
  }
?></div>
                  <h3>Priorities</h3>
                </div>
              </div>
              <div class="animated flipInY col-lg-3 col-md-3 col-sm-6 col-xs-12">
                <div class="tile-stats">
                  <div class="icon"><i class="fa fa-check-square-o"></i></div>
                  <div class="count">
                    <!-- Ready jobs -->

                <?php
include "./php/config.php";
$result = mysqli_query($con, "SELECT COUNT(DISTINCT JobID) FROM Ready_list") or die(mysqli_error($con));

if ($result->num_rows > 0) {
    // output data
  while($row = $result->fetch_assoc()) {

        echo $row["COUNT(DISTINCT JobID)"];
                  
    }
  }
?></div>
                  <h3>Completed Jobs</h3>
                </div>
              </div>
            </div>

              
            </div>

          </div>
          <br />

           
        <!-- /page content -->

  
      </div>
    </div>

    <!-- jQuery -->
    <script src="../vendors/jquery/dist/jquery.min.js"></script>
    <!-- Bootstrap -->
    <script src="../vendors/bootstrap/dist/js/bootstrap.min.js"></script>
    <!-- FastClick -->
    <script src="../vendors/fastclick/lib/fastclick.js"></script>
    <!-- NProgress -->
    
    <!-- gauge.js -->
    <script src="../vendors/gauge.js/dist/gauge.min.js"></script>
    <!-- bootstrap-progressbar -->
    <script src="../vendors/bootstrap-progressbar/bootstrap-progressbar.min.js"></script>
    <!-- iCheck -->
    <script src="../vendors/iCheck/icheck.min.js"></script>
    <!-- Skycons -->
    <script src="../vendors/skycons/skycons.js"></script>
    <!-- Flot -->
    <script src="../vendors/Flot/jquery.flot.js"></script>
    <script src="../vendors/Flot/jquery.flot.pie.js"></script>
    <script src="../vendors/Flot/jquery.flot.time.js"></script>
    <script src="../vendors/Flot/jquery.flot.stack.js"></script>
    <script src="../vendors/Flot/jquery.flot.resize.js"></script> 
    <!-- DateJS -->
    <script src="../vendors/DateJS/build/date.js"></script>
    <!-- Custom Theme Scripts -->
    <script src="../build/js/custom.min.js"></script>
	
  </body>
</html>
