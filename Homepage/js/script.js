var app = angular.module('nvd3', []);

app
.controller('MainCtrl', function($scope, $timeout, DataService) {	
	$scope.dashboard = {
		widgets: [{
			col: 0,
			row: 0,
			sizeY: 2,
			sizeX: 2,
			name: "Discrete Bar Chart",
			chart: {
			  options: DataService.discreteBarChart.options(),
			  data: DataService.discreteBarChart.data(),
			  api: {} 
			}
		}, {
			col: 2,
			row: 0,
			sizeY: 2,
			sizeX: 2,
			name: "Candlestick Bar Chart",
			chart: {
			  options: DataService.candlestickBarChart.options(),
			  data: DataService.candlestickBarChart.data(),
			  api: {} 
			}
		}, {
			col: 0,
			row: 2,
			sizeY: 2,
			sizeX: 3,
			name: "Line Chart",
			chart: {
			  options: DataService.lineChart.options(),
			  data: DataService.lineChart.data(),
			  api: {} 
			}
		}, {
			col: 4,
			row: 2,
			sizeY: 1,
			sizeX: 1,
			name: "Pie Chart",
			chart: {
			  options: DataService.pieChart.options(),
			  data: DataService.pieChart.data(),
			  api: {} 
			}
		}]
	};
  
  // We want to manually handle `window.resize` event in each directive.
  // So that we emulate `resize` event using $broadcast method and internally subscribe to this event in each directive
  // Define event handler
  $scope.events = {
    resize: function(e, scope){
      $timeout(function(){
        scope.api.update()
      },200)
    }
  };
  angular.element(window).on('resize', function(e){
    $scope.$broadcast('resize');
  });
  
  // We want to hide the charts until the grid will be created and all widths and heights will be defined.
  // So that use `visible` property in config attribute
  $scope.config = {
    visible: false
  };
  $timeout(function(){
    $scope.config.visible = true;
  }, 200);
});
// JavaScript Document