# API Routes
## Responses structures
###  On success:
```json
{
    "success":true,
    "message":"some text",
    "data"{
        "custom":"object",
        "volum":true
    }
}
```  
Note that the message and data fields can be empty. The specific routes will indicate whether you should expect a data structure in the response.  
   
### On failures:
```json
{
    "sucess":false,
    "error": "description"
}
```
In this case, you'll always get an error description.


## User auth
### Get user details
**GET** /user  
**Response datae**: (Contained in $.data)  
```json
{
    "firstname":"<their_firstname>",
    "lastname":"<their_lastname>",
    "email":"<their_email>",
    "id":0,
}
```
**Note**: If success is false, it's most likely the user isn't connected. Send them to the connection provider.