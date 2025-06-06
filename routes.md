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
**Response data**: (Contained in $.data)  
```json
{
    "name":"<their_name>",
    "email":"<their_email>",
    "class_name":"<their_class>",
    "id":0,
    "is_admin": false
}
```
**Note**: If success is false, it's most likely the user isn't connected. Send them to the connection provider.

## VMs management
### List VMs
**GET** /vms
**Response data**: 
```json
[
    {
        "status": "online",
        "name": "super-vm",
        "vmid": "2000",
        "cpus": "2",
        "maxmem": "100000000",
        "maxdisk": "100000000",
        "maxswap": "100000000",
        "uptime": "100000000",
        "cpu": "0.2454",
        "disk": "100000000",
        "mem": "100000000",
        "swap": "100000000",
        "netin": "100000000",
        "netout": "100000000",
        "ip_address": "10.0.1.5",
        "subdomain": "test",
        "owner_id": 1
    }
]
```
**Notes**: 
- if the user is admin, it will return all the available VMs.

## VMs management
### List VMs
**GET** /vms/:id
**Response data**: 
```json
[
    {
        "status": "online",
        "name": "super-vm",
        "vmid": "2000",
        "cpus": "2",
        "maxmem": "100000000",
        "maxdisk": "100000000",
        "maxswap": "100000000",
        "uptime": "100000000",
        "cpu": "0.2454",
        "disk": "100000000",
        "mem": "100000000",
        "swap": "100000000",
        "netin": "100000000",
        "netout": "100000000",
        "ip_address": "10.0.1.5",
        "subdomain": "test",
        "owner_id": 1
    }
]
```
**Notes**: 
- If the user isn't owner of the vm + isn't admin, it will return the "unkown vm" error.
- If the vm doesn't exists, it will return the "unknown vm" error.

### Create a VM
**POST** /vms/create
**BODY**: `{"server_name":"the-name", "subdomain":"supercool"}`
**Response data**:
```json
[
    {
        "status": "online",
        "name": "super-vm",
        "vmid": "2000",
        "cpus": "2",
        "maxmem": "100000000",
        "maxdisk": "100000000",
        "maxswap": "100000000",
        "uptime": "100000000",
        "cpu": "0.2454",
        "disk": "100000000",
        "mem": "100000000",
        "swap": "100000000",
        "netin": "100000000",
        "netout": "100000000",
        "ip_address": "10.0.1.5",
        "subdomain": "supercool",
        "password":"auto-generated",
        "owner_id": 1
    }
]
```

### Stop a VM
**POST** /vms/:id/stop

### Start a VM
**POST** /vms/:id/start

### Delete a VM
**POST** /vms/:id
**Note**: Yes, i know "delete" is the standard, but I couldn't get the CORS to work with the delete method (??)
