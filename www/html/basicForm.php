
<!DOCTYPE html>
<html>
<head>
<style>
body {
	display: flex;
	justify-content: center;
	align-items: center;
	height: 100vh;
	background-color: #f0f0f0;
}

form {
	border: 1px solid #000;
	padding: 20px;
	background-color: #fff;
	text-align: center;
}

input[type="submit"] {
	background-color: #4CAF50;
	color: white;
	padding: 10px 20px;
	margin: 10px 0;
	border: none;
	cursor: pointer;
	width: 100%;
}

input[type="submit"]:hover {
    opacity: 0.8;
}

.output {
    margin-top: 20px;
    font-weight: bold;
}
</style>
</head>
<body>

<form method="post" action="<?php echo $_SERVER['PHP_SELF'];?>">
	Name: <input type="text" name="name"> <br><br>
	Email: <input type="text" name="email"> <br><br>
	Password: <input type="password" name="password"> <br><br>
	<input type="submit">
</form>

<div class="output">
<?php
if ($_SERVER["REQUEST_METHOD"] == "POST") {
	// collect value of input field
	$name = $_POST['name'];
	$email = $_POST['email'];
	$password = $_POST['password'];
	if (empty($name)) {
		echo "Name is empty <br>";
	} else {
		echo "Name: ".$name."<br>";
	}
	if (empty($email)) {
		echo "Email is empty <br>";
	} else {
		echo "Email: ".$email."<br>";
	}
	if (empty($password)) {
		echo "Password is empty <br>";
	} else {
		echo "Password: ".$password."<br>";
	}

	// Print out the contents of the $_POST array
	echo "POST data: ";
	print_r($_POST);
}
?>
</div>

</body>
</html>

