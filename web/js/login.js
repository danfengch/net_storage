
function check_login_status()
{
	var login_Status = $.cookie('login_Status');
	//alert(login_Status);
	if("true" != login_Status)	
	{
		alert("请先登录本系统！");
		window.top.location='login.htm';
		return false;
	}		
}