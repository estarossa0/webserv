<?php

if (isset($_GET['path']))
	unset($_COOKIE['cgi']);

?>
<!DOCTYPE html>
<html lang="en">
<head>
	<meta charset="UTF-8">
	<meta http-equiv="X-UA-Compatible" content="IE=edge">
	<meta name="viewport" content="width=device-width, initial-scale=1.0">
	<link rel="stylesheet" href="https://cdn.jsdelivr.net/npm/bootstrap@5.0.2/dist/css/bootstrap.min.css" integrity="sha384-EVSTQN3/azprG1Anm3QDgpJLIm9Nao0Yz1ztcQTwFspd3yD65VohhpuuCOmLASjC" crossorigin="anonymous">
	<title>Document</title>
	<style>
		html {
			font-family: sans-serif;
			height: 100%;
		}
		body {
			height: 100vh;
		}
		.size {
			font-size: 66px;
			color: white;
		}
		.-mt {
			margin-top: -30px;
		}
	</style>
</head>

<body class="bg-info overflow-hidden">
	<h1 class="text-white text-center">webserv</h1>
	<div class="mx-auto h-75 d-flex w-100 justify-content-center -mt">
		<div class="d-flex flex-column my-auto">
			<h1 class="size text-center">
				<?php 
					if (!isset($_COOKIE['cgi'])) { 
						echo "Not logged in";
					} else 
						echo "Welcome home";
					?>
				</h1>
				<?php 
				if (!isset($_COOKIE['cgi']))
					echo '<a href="login.php" class="btn btn-primary text-center mt-5 text-decoration-none">Login</a>';
				else
					echo '<a href="home.php?path=logout" class="btn btn-primary text-center mt-5 text-decoration-none">Logout</a>';
				?>
		</div>
	</div>
</body>
</html>