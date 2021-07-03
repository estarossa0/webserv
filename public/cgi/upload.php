<?php
if(isset($_POST["submit"])) {
	$target_dir = "../upload/";
	$target_file = $target_dir . basename($_FILES["file"]["name"]);
	$uploadOk = 1;
	$imageFileType = strtolower(pathinfo($target_file,PATHINFO_EXTENSION));
	
	if (file_exists($target_file)) {
		echo "Sorry, file already exists.";
		$uploadOk = 0;
	}
	
	if ($_FILES["file"]["size"] > 500000) {
		echo "Sorry, your file is too large.";
		$uploadOk = 0;
	}
	
	if ($uploadOk == 0) {
		echo "Sorry, your file was not uploaded.";
	} else {
		if (move_uploaded_file($_FILES["file"]["tmp_name"], $target_file)) {
			echo "The file ". htmlspecialchars( basename( $_FILES["file"]["name"])). " has been uploaded.";
		} else {
			echo "Sorry, there was an error uploading your file.";
		}
	}
}
?>

<!DOCTYPE html>
<html lang="en">

<head>
	<meta charset="UTF-8">
	<meta http-equiv="X-UA-Compatible" content="IE=edge">
	<meta name="viewport" content="width=device-width, initial-scale=1.0">
	<link rel="stylesheet" href="https://cdn.jsdelivr.net/npm/bootstrap@5.0.2/dist/css/bootstrap.min.css"
		integrity="sha384-EVSTQN3/azprG1Anm3QDgpJLIm9Nao0Yz1ztcQTwFspd3yD65VohhpuuCOmLASjC" crossorigin="anonymous">
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
            <?php

                if (isset($_POST['submit']))
                    echo "<h2 class=\"text-center text-white\"> File uploaded </h2>";
            ?>
			<h1 class="size text-center">UPLOAD FORM</h1>
			<form class="mt-5 d-flex flex-column" action="upload.php" method="post" enctype="multipart/form-data">
				<input class="mb-3" type="file" name="file" />
				<input type="submit" class="btn btn-primary" name="submit" value="Submit" />
			</form>
		</div>
	</div>
</body>

</html>