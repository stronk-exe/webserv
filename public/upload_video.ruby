require 'net/http'

def post_video(url, video_file_path)
  uri = URI.parse(url)
  video_data = File.read(video_file_path)

  Net::HTTP.start(uri.host, uri.port) do |http|
    req = Net::HTTP::Post.new(uri.path)
    req.body = video_data
    req.content_type = 'video/mp4' # Set the appropriate content type for your video file
    response = http.request(req)
    puts "Response: #{response.body}"
  end
end

# Usage:
post_video('http://example.com/upload', 'path/to/video.mp4')
