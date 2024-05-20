# ObjectPool Design Pattern


What is Object Pool?
-> First we need to understand what is Pool.

    Pool:
        - Pool is a collection of something. Here Collection of objects.

-> Object pool is a limited number of objects which are already created when we initialize the Object Pool class object.

-> In Real life, this should be Singleton Class.

-> In my implementation, when pool is empty (i.e. All objects are already aquired) then I am creating new object of that class.
    But In your application, you might want to restrict to create the fix number of objects.
    
-> For restricting on creating of class object, you can add condition and wait till some object gets released.

Difference between Object pool and Thread pool:

-> In Object pool, client(user) class aquire the object from ObjectPool and do the required task.

-> While In case of Thread pool, client(user) delegate the task to the thread pool and thread pool will execute the given method.

    Appllication of Object Pool:
    
    e.g. 
    
        1.) Datanbase connection pool.
            -> Because, Application should restrict the number of connection to the DB. Also, creating DB instance is also bulky task.
            -> As, number of connection instances will increase, complexity to execute query at DB side will increase to achieve CAP and ACID property.
            
        2.) Websocket Connection pool from Stock Broker (like Zerodha, Upstox) to NSE/BSE
            -> If stock broker creates unlimited number of web socket connection with NSE/BSE then it will be overwhelm with the number of connection and it can not handle.

        3.) If object creation is time consuming and bulky then we should use Object Pool.

