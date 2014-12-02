
$(document).ready( function()
{
  var  dt;				// Last modified date
  var  dt_fmt;				// Formatted last modified date
  var  dt_mth = [			// Short month names
         "Jan", "Feb", "Mar", "Apr", "May", "Jun",
         "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
       ];

  //  Update last modified date

  dt = new Date( document.lastModified );

  dt_fmt = ("0" + dt.getDate()).slice( -2 );
  dt_fmt = dt_fmt + "-" + dt_mth[ dt.getMonth() ];
  dt_fmt = dt_fmt + "-" + dt.getFullYear().toString().slice( -2 );

  $("#mod-date").text( dt_fmt );
} );
