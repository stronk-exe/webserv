<?php
// Check if a file was uploaded
if ($_FILES['video']['name']) {
    $file_name = $_FILES['video']['name'];
    $file_size = $_FILES['video']['size'];
    $file_tmp = $_FILES['video']['tmp_name'];
    $file_type = $_FILES['video']['type'];

    // Specify the directory where you want to save the uploaded videos
    $upload_directory = 'uploads/';

    // Check if the directory exists, if not, create it
    if (!is_dir($upload_directory)) {
        mkdir($upload_directory, 0755, true);
    }

    // Move the uploaded file to the specified directory
    $target_file = $upload_directory . $file_name;
    move_uploaded_file($file_tmp, $target_file);

    // Display a success message
    echo "Video uploaded successfully!";
}
?>

<!DOCTYPE html>
<html>
<head>
    <title>Video Upload</title>
</head>
<body>
    <form action="" method="POST" enctype="multipart/form-data">
        <input type="file" name="video">
        <input type="submit" value="Upload">
    </form>
</body>
</html>