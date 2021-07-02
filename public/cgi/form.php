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

                if (isset($_POST['name']))
                {
                    echo "<h2 class=\"text-center text-white\"> Welcome </h2>";
                    echo "<p class=\"text-center\"> name: " . $_POST['name'];
                    if (isset($_POST['email']))
                        echo " | email: " . $_POST['email'];
                    echo "</p>";
                }

                if (isset($_POST['name']) && !isset($_POST['email']))
                    echo "<h2 class=\"text-center\">email is required</h2>";
            ?>
			<h1 class="size text-center">POST FORM</h1>
			<form class="mt-5 d-flex flex-column" action="form.php" method="post">
				<div class="input-group mb-3">
					<div class="input-group-prepend">
						<span class="input-group-text" id="basic-addon1">Name</span>
					</div>
					<input type="text" class="form-control" placeholder="Username" aria-label="Username" aria-describedby="basic-addon1" name="name">
				</div>
				<div class="input-group mb-3">
					<div class="input-group-prepend">
						<span class="input-group-text" id="basic-addon1">@</span>
					</div>
					<input type="text" class="form-control" placeholder="Email" aria-label="Email" aria-describedby="basic-addon1" name="email">
				</div>
				<input type="submit" class="btn btn-primary" value="Submit" />
			</form>
		</div>
	</div>
</body>

</html>