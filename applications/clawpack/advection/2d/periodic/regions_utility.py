# ==============================================================================
#  Base data class for Clawpack data objects

import numpy as np

class ClawData(object):
    r"""
    Class to be subclassed when defining data objects that should have
    a limited set of allowed attributes.  Useful to guard against
    typos or misrembering the names of expected attributes.

    Resetting values of existing attributes is allowed as usual,
    but new attributes can only be added using the method add_attribute.

    Trying to set a nonexistent attribute will raise an AttributeError
    exception, except for those starting with '_'.   
    """


    def __init__(self, attributes=None):
        
        # Attribute to store a list of the allowed attributes, 
        # appended to when add_attribute is used: 
        object.__setattr__(self,'_attributes',[])

        # Output file handle
        object.__setattr__(self,'_out_file',None)

        # Initialize from attribute list provided
        if attributes:
            for attr in attributes:
                self.add_attribute(attr,None)


    def __setattr__(self,name,value):
        r"""
        Check that attribute exists before setting it.
        If not, raise an AttributeError.
        Exception: attributes starting with '_' are ok to set.
        """

        if (not name in self._attributes) and (name[0] != '_'):
            print("*** Unrecognized attribute: ",name)
            print("*** Perhaps a typo?")
            print("*** Add new attributes using add_attribute method")
            raise AttributeError("Unrecognized attribute: %s" % name)
        
        # attribute exists, ok to set:
        object.__setattr__(self,name,value)


    def __str__(self):
        r"""Returns string representation of this object"""
        output = "%s%s\n" % ("Name".ljust(25),"Value".ljust(12))
        for (k,v) in self.items():
            output += "%s%s\n" % (str(k).ljust(25),str(v).ljust(12))
        return output


    def add_attribute(self, name, value=None, add_to_list=True):
        r"""
        Adds an attribute called name to the data object

        If an attribute needs to be added to the object, this routine must be
        called or the attribute will not be written out.

        :Input:
         - *name* - (string) Name of the data attribute
         - *value* - (id) Value to set *name* to, defaults to None
        """
        if (name not in self._attributes) and add_to_list:
            self._attributes.append(name)
        object.__setattr__(self,name,value)


    def add_attributes(self, arg_list, value=None):
        r"""
        Add a list of attributes, each initialized to *value*.
        """
        for name in arg_list:
            self.add_attribute(name, value)


    def remove_attributes(self, arg_list):
        r"""
        Remove the listed attributes.
        """

        # Convert to list if args is not already a list
        if not isinstance(arg_list,list):
            arg_list = [arg_list]

        for arg in arg_list:
            self._attributes.remove(arg)
            delattr(self,arg)


    def attributes(self):
        r"""Returns tuple of attribute names"""
        return tuple(self._attributes)


    def has_attribute(self,name):
        r"""
        Check if this data object has the given attributes

        :Input:
         - *name* - (string) Name of attribute

        :Output:
         - (bool) - True if data object contains a data attribute name
        """
        return name in self._attributes

        
    def iteritems(self):
        r"""
        Returns an iterator of attributes and values from this object

        :Output:
         - (Iterator) Iterator over attributes and values
        """
        return [(k,getattr(self,k)) for k in self._attributes]


    def open_data_file(self, name, datasource='setrun.py'):
        """
        Open a data file and write a warning header.
        Warning header starts with '#' character.  These lines are skipped if
        data file is opened using the library routine opendatafile.

        :Input:
         - *name* - (string) Name of data file
         - *datasource* - (string) Source for the data

        :Output:
         - (file) - file object
        """

        source = datasource.ljust(25)
        self._out_file = open(name, 'w')
        self._out_file.write('########################################################\n')
        self._out_file.write('### DO NOT EDIT THIS FILE:  GENERATED AUTOMATICALLY ####\n')
        self._out_file.write(f'### To modify data, edit {source:25s}  ####\n')
        self._out_file.write('###                                                 ####\n')
        self._out_file.write('########################################################\n\n')


    def close_data_file(self):
        r"""Close output data file"""
        self._out_file.close()
        self._out_file = None


    def write(self, out_file, data_source='setrun.py'):
        r"""Write out all data files in this ClawData object"""

        # Open data file
        self.open_data_file(out_file,data_source)

        # Write out list of attributes
        for (name,value) in self.iteritems():
            self.data_write(name)


    def data_write(self, name=None, value=None, alt_name=None, description=''):
        r"""
        Write out value to data file, in the form ::

           value =: name   # [description]

        Remove brackets and commas from lists, and replace booleans by T/F.

        :Input:
         - *name* - (string) normally a string defining the variable,
           ``if name==None``, write a blank line.
         - *description* - (string) optional description
        """
        if self._out_file is None:
            raise Exception("No file currently open for output.")

        # Defaults to the name of the variable requested
        if alt_name is None:
            alt_name = name

        if name is None and value is None:
            # Write out a blank line
            self._out_file.write('\n')
        else:
            # Use the value passed in instead of fetching from the data object
            if value is None:
                value = self.__getattribute__(name)

            # Convert value to an appropriate string repr
            if isinstance(value,np.ndarray):
                value = list(value)
            if isinstance(value,tuple) | isinstance(value,list):
                # Remove [], (), and ','
                string_value = repr(value)[1:-1]
                string_value = string_value.replace(',','')
            elif isinstance(value,bool):
                if value:
                    string_value = 'T'
                else:
                    string_value = 'F'
            else:
                string_value = repr(value)
            padded_value = string_value.ljust(20)
            padded_name = alt_name.ljust(20)
            if description != '':
                self._out_file.write('%s =: %s # %s \n' % 
                                        (padded_value, padded_name, description))
            else:
                self._out_file.write('%s =: %s\n' % 
                                    (padded_value, padded_name))
  

    def read(self,path,force=False):
        r"""Read and fill applicable data attributes.

        Note that if the data attribute is not found an exception will be
        raised unless the force argument is set to True in which case a new
        attribute will be added.
        """

        data_file = open(os.path.abspath(path),'r')

        for lineno,line in enumerate(data_file):
            if "=:" not in line:
                continue

            value, tail = line.split("=:")
            varname = tail.split()[0]

            # Set this parameter
            if self.has_attribute(varname) or force:
                value = self._parse_value(value)
                if not self.has_attribute(varname):
                    self.add_attribute(varname,value)
                else:
                    setattr(self,varname,value)
    

    def _parse_value(self,value):
        r"""
        Attempt to make sense of a value string from a config file.  If the
        value is not obviously an integer, float, or boolean, it is returned as
        a string stripped of leading and trailing whitespace.

        :Input:
            - *value* - (string) Value string to be parsed

        :Output:
            - (id) - Appropriate object based on *value*
        """
        value = value.strip()
        if not value:
            return None

        # assume that values containing spaces are lists of values
        if len(value.split()) > 1:
            return [self._parse_value(vv) for vv in value.split()]

        try:
            # see if it's an integer
            value = int(value)
        except ValueError:
            try:
                # see if it's a float
                value = float(value)
            except ValueError:
                # see if it's a bool
                if value[0] == 'T':
                    value = True
                elif value[0] == 'F':
                    value = False

        return value

#  Base data class for Clawpack data objects
# ==============================================================================


# ==============================================================================
#  Gauge data object
# class GaugeData(clawpack.clawutil.data.ClawData):
class GaugeData(ClawData):
    r""""""

    defaults = {"min_time_increment":0.0}

    @property
    def gauge_numbers(self):
        if len(self.gauges) == 1:
            return [self.gauges[0][0]]
        else:
            return [gauge[0] for gauge in self.gauges]


    def __init__(self,dim, min_time_increment=0):
        super(GaugeData,self).__init__()

        self.add_attribute('min_time_increment',0.0)
        self.add_attribute('dim',dim)
        self.add_attribute('gauges',[])
        self.add_attribute('_out_file',None)

    def write(self, out_file='gauges.data',\
              data_source='write_gauges.py'):
        r"""Write out gauge information data file."""

        # Write out gauge data file
        self.open_data_file(out_file,data_source)
        self.data_write(name="dimension",value=self.dim)
        self.data_write(name='ngauges',value=len(self.gauges),)
        for gauge in self.gauges:
            num = len(gauge) - (1 + self.dim)
            fstr = "{:4d} " + "{:17.10e} "*num + self.dim*" {:13.6e}" + "\n"
            self._out_file.write(fstr.format(*gauge))
        self.data_write()   # Write '\n'

        self.data_write(name='min_time_increment',value=self.min_time_increment)
        
        self.close_data_file()

#  Gauge data objects
# ==============================================================================


# ==============================================================================
#  Region data object
class RegionData(ClawData):
    r""""""

    def __init__(self,dim=None):

        super(RegionData,self).__init__()

        self.add_attribute('regions',[])
        self.add_attribute('dim',dim)

    def write(self,out_file='regions.data',data_source='setrun.py'):


        self.open_data_file(out_file,data_source)
        self.data_write(name="dimension",value=self.dim)
        self.data_write(value=len(self.regions),alt_name='num_regions')
        for region in self.regions:
            # write minlevel,maxlevel as integers, remaining floats in e format:
            num = 2*(1+self.dim)
            fstr = "{:4d} "*2 + "{:16.6e} "*num + "\n"
            self._out_file.write(fstr.format(*region))
            
        self.close_data_file()

#  Region data object
# ==============================================================================

