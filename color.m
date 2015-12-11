function varargout = color(varargin)
% COLOR MATLAB code for color.fig
%      COLOR, by itself, creates a new COLOR or raises the existing
%      singleton*.
%
%      H = COLOR returns the handle to a new COLOR or the handle to
%      the existing singleton*.
%
%      COLOR('CALLBACK',hObject,eventData,handles,...) calls the local
%      function named CALLBACK in COLOR.M with the given input arguments.
%
%      COLOR('Property','Value',...) creates a new COLOR or raises the
%      existing singleton*.  Starting from the left, property value pairs are
%      applied to the GUI before color_OpeningFcn gets called.  An
%      unrecognized property name or invalid value makes property application
%      stop.  All inputs are passed to color_OpeningFcn via varargin.
%
%      *See GUI Options on GUIDE's Tools menu.  Choose "GUI allows only one
%      instance to run (singleton)".
%
% See also: GUIDE, GUIDATA, GUIHANDLES

% Edit the above text to modify the response to help color

% Last Modified by GUIDE v2.5 11-Dec-2015 13:17:25

% Begin initialization code - DO NOT EDIT
gui_Singleton = 1;
gui_State = struct('gui_Name',       mfilename, ...
                   'gui_Singleton',  gui_Singleton, ...
                   'gui_OpeningFcn', @color_OpeningFcn, ...
                   'gui_OutputFcn',  @color_OutputFcn, ...
                   'gui_LayoutFcn',  [] , ...
                   'gui_Callback',   []);
if nargin && ischar(varargin{1})
    gui_State.gui_Callback = str2func(varargin{1});
end

if nargout
    [varargout{1:nargout}] = gui_mainfcn(gui_State, varargin{:});
else
    gui_mainfcn(gui_State, varargin{:});
end
% End initialization code - DO NOT EDIT


% --- Executes just before color is made visible.
function color_OpeningFcn(hObject, eventdata, handles, varargin)
% This function has no output args, see OutputFcn.
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
% varargin   command line arguments to color (see VARARGIN)

% Choose default command line output for color
handles.output = hObject;
handles.files = {};

[handles.files, labels] = dir_read('../Data/');

if length(labels) == 0
    fprintf(' Change argumnet in dir_read inside color.m ');
end
for i = 1:length(handles.files)
    handles.X{i} = char(handles.files(i));
end

handles.current_img = imread(handles.X{1});
imshow(handles.current_img);
handles.index = 1;
Cek(hObject, eventdata, handles);
% Update handles structure
guidata(hObject, handles);

% UIWAIT makes color wait for user response (see UIRESUME)
% uiwait(handles.figure1);


% --- Outputs from this function are returned to the command line.
function varargout = color_OutputFcn(hObject, eventdata, handles) 
% varargout  cell array for returning output args (see VARARGOUT);
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Get default command line output from handles structure
varargout{1} = handles.output;


% --- Executes when figure1 is resized.
function figure1_ResizeFcn(hObject, eventdata, handles)
% hObject    handle to figure1 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)


% --- Executes on slider movement.
function slider1_Callback(hObject, eventdata, handles)
% hObject    handle to slider1 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'Value') returns position of slider
%        get(hObject,'Min') and get(hObject,'Max') to determine range of slider


% --- Executes during object creation, after setting all properties.
function slider1_CreateFcn(hObject, eventdata, handles)
% hObject    handle to slider1 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: slider controls usually have a light gray background.
if isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor',[.9 .9 .9]);
end


% --- Executes on button press in pushbutton1.
function pushbutton1_Callback(hObject, eventdata, handles)
% hObject    handle to pushbutton1 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
handles.output = hObject;
handles.index = handles.index - 1;
Cek(hObject, eventdata, handles);
handles.current_img = imread(handles.X{handles.index});
imshow(handles.current_img);
guidata(hObject, handles);

% --- Executes on button press in pushbutton2.
function pushbutton2_Callback(hObject, eventdata, handles)
% hObject    handle to pushbutton2 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
handles.output = hObject;
handles.index = handles.index + 1;
Cek(hObject, eventdata, handles);
handles.current_img = imread(handles.X{handles.index});
imshow(handles.current_img);
guidata(hObject, handles);

function Cek(hObject, eventdata, handles)
handles.output = hObject;
n = length(handles.files);
if handles.index > 1,  set(handles.pushbutton1,'enable','on');
else                           set(handles.pushbutton1,'enable','off'); end
if handles.index < n, set(handles.pushbutton2,'enable','on');
else                           set(handles.pushbutton2,'enable','off'); end
guidata(hObject, handles);
