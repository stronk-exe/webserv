<?php
if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    // Specify the directory where the uploaded videos will be stored
    $targetDir = 'uploads/a/';

    // Check if the uploads directory exists, create it if not
    if (!file_exists($targetDir)) {
        mkdir($targetDir, 0777, true);
    }

    // Check if a file was uploaded without any errors
    if (isset($_FILES['video']) && $_FILES['video']['error'] === UPLOAD_ERR_OK) {
        $videoName = $_FILES['video']['name'];
        $videoTmp = $_FILES['video']['tmp_name'];
        $videoSize = $_FILES['video']['size'];

        // Generate a unique name for the video
        $videoPath = $targetDir . uniqid() . '_' . $videoName;

        // Move the uploaded video to the specified directory
        if (move_uploaded_file($videoTmp, $videoPath)) {
            echo "Video uploaded successfully!";
        } else {
            echo "Error uploading the video.";
        }
    } else {
        echo "No video file was uploaded or an error occurred.";
    }
}
?>

<!DOCTYPE html>
<html>
<head>
    <title>Video Upload</title>
</head>
<body>
    <form method="POST" enctype="multipart/form-data">
        <input type="file" name="video">
        <input type="submit" value="Upload">
    </form>
</body>
</html>
