<?php
if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    $targetDir = "../Uploads/";
    $targetFile = $targetDir . basename($_FILES["file"]["name"]);
    $uploadOk = 1;
    $imageFileType = strtolower(pathinfo($targetFile, PATHINFO_EXTENSION));

    // Check if it's an image or video file
    $allowedImageTypes = array("jpg", "jpeg", "png", "gif");
    $allowedVideoTypes = array("mp4", "avi", "mov");

    if (in_array($imageFileType, $allowedImageTypes)) {
        // It's an image file
        $fileType = "image";
    } elseif (in_array($imageFileType, $allowedVideoTypes)) {
        // It's a video file
        $fileType = "video";
    } else {
        echo "Invalid file type. Only images and videos are allowed.";
        $uploadOk = 0;
    }

    // Check file size
    if ($_FILES["file"]["size"] > 5000000) { // 5MB limit
        echo "File is too large. Maximum file size allowed is 5MB.";
        $uploadOk = 0;
    }

    // Upload file if everything is okay
    if ($uploadOk == 1) {
        if (move_uploaded_file($_FILES["file"]["tmp_name"], $targetFile)) {
            echo "The file has been uploaded successfully.";

            // You can perform further actions here, like storing the file details in a database

        } else {
            echo "An error occurred while uploading the file.";
        }
    }
}
?>

<!DOCTYPE html>
<html>
<head>
    <title>File Upload</title>
</head>
<body>
    <form action="<?php echo $_SERVER["PHP_SELF"]; ?>" method="post" enctype="multipart/form-data">
        <label for="file">Select file:</label>
        <input type="file" name="file" id="file" required>
        <input type="submit" value="Upload">
    </form>
</body>
</html>
