#include <stdio.h>
#include <curl/curl.h>

int publish_to_slack(int temperature)
{
  CURL *curl;
  CURLcode res;
  char api_url[200];

  curl = curl_easy_init();
  if(curl) {
	sprintf(api_url,"https://aq6gfofueh.execute-api.us-east-1.amazonaws.com/dev?type=0&team_id=T2HCD11DY&temperature=%d",temperature);
    curl_easy_setopt(curl, CURLOPT_URL, api_url);
    /* example.com is redirected, so we tell libcurl to follow redirection */
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

    /* Perform the request, res will get the return code */
    res = curl_easy_perform(curl);
    /* Check for errors */
    if(res != CURLE_OK)
      fprintf(stderr, "curl_easy_perform() failed: %s\n",
              curl_easy_strerror(res));

    /* always cleanup */
    curl_easy_cleanup(curl);
  }
  return 0;
}
/*int main (){
	publish_to_slack(31);	
}*/
