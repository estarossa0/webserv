<!DOCTYPE html>
<html lang="en">
<head>
	<meta charset="UTF-8">
	<meta http-equiv="X-UA-Compatible" content="IE=edge">
	<meta name="viewport" content="width=device-width, initial-scale=1.0">
	<link rel="stylesheet" href="https://cdn.jsdelivr.net/npm/bootstrap@5.0.2/dist/css/bootstrap.min.css" integrity="sha384-EVSTQN3/azprG1Anm3QDgpJLIm9Nao0Yz1ztcQTwFspd3yD65VohhpuuCOmLASjC" crossorigin="anonymous">
	<title>Document</title>
</head>
<body>
<div class="container">
		<div class="d-flex flex-row justify-content-center">
			<h1>
			<?php 
					if (!isset($_COOKIE['cgi'])) { 
						echo "Not logged in";
					} else 
						echo "Welcome home";
				?>
			</h1>
		</div>
	</div>
</body>
</html>
