<!DOCTYPE html>
<html>
<head>
<style>
body {
    background-color: #f0f8ff;
    font-family: Arial, sans-serif;
    margin: 0;
    padding: 0;
    display: flex;
    justify-content: center;
    align-items: center;
    height: 100vh;
}

form {
    background-color: #fff;
    padding: 20px;
    border-radius: 10px;
    box-shadow: 0px 0px 10px 2px rgba(0,0,0,0.1);
}

input[type="file"] {
    margin-bottom: 10px;
}

input[type="submit"] {
    background-color: #4CAF50;
    color: white;
    padding: 10px 20px;
    border: none;
    border-radius: 5px;
    cursor: pointer;
}

input[type="submit"]:hover {
    background-color: #45a049;
}
</style>
</head>
<body>

<form action="<?php echo htmlspecialchars($_SERVER["PHP_SELF"]);?>" method="post" enctype="multipart/form-data">
    <h2>Select file to upload:</h2>
    <input type="file" name="filename" id="filename">
    <input type="submit" value="Upload File" name="submit">
</form>

</body>
</html>

<?php
if ($_SERVER["REQUEST_METHOD"] == "POST") {
    $target_dir = "../../data/";
    $target_file = $target_dir . basename($_FILES["filename"]["name"]);

    if (move_uploaded_file($_FILES["filename"]["tmp_name"], $target_file)) {
        echo "The file " . htmlspecialchars(basename($_FILES["filename"]["name"])) . " has been uploaded.";
    } else {
        echo "Sorry, there was an error uploading your file.";
    }
}
?>
